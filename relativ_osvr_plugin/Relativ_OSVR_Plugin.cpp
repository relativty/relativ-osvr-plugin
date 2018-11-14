// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/PluginKit/ButtonInterfaceC.h>

//Winapi
#include <windows.h>

// Standard includes
#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>

// Third party includes
#include <json/json.h>
#include <serial/serial.h>

// Json header
#include "Relativ_OSVR_Plugin.h"

#define DEVICE_NAME "Relativ"

namespace {
	std::atomic<OSVR_Quaternion> relativ_rotation;
	std::mutex msg_lock;
	std::vector<std::string> msg_queue;
	std::thread relativ_thread;
	bool keep_thread_open = true;

	void add_msg(std::string msg) {
		msg_lock.lock();
		msg_queue.push_back(msg);
		msg_lock.unlock();
	}

	bool get_last_msg(std::string* s) {
		if (msg_queue.size() == 0) {
			return false;
		}
		else {
			msg_lock.lock();
			(*s) = msg_queue.at(msg_queue.size() - 1);
			msg_queue.pop_back();
			msg_lock.unlock();
			return true;
		}
	}

	void relativ_connect(std::string port) {
		bool was_connected = false;
		while (keep_thread_open) {
			serial::Serial relativ;
			std::string last_recv;
			try {
				relativ.setTimeout(serial::Timeout::simpleTimeout(1000));
				relativ.setPort(port);
				relativ.setBaudrate(115200);
				while (!relativ.isOpen()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					relativ.open();
				}
				add_msg("Connected!");
				while (relativ.isOpen() && keep_thread_open) {
					was_connected = true;
					if (last_recv.size() > 0 && last_recv[0] != 0) {
						if (GetAsyncKeyState(VK_SHIFT) != 0 && GetAsyncKeyState(VK_CONTROL) != 0 && GetAsyncKeyState(0x49) != 0) {
							relativ.write("C\n");
						}
					}
					last_recv = relativ.readline();
					std::string raw_str = last_recv;
					if (raw_str.size() > 0) {
						if (raw_str[0] != 0) {
							if (raw_str.size() > 3) {
								bool valid_reading = true;
								float read_vals[4] = { 2,2,2,2 }; // Quat values will never be greater/less than 1,-1
								try {
									size_t pos = 0;
									pos = raw_str.find(',');
									read_vals[0] = std::stof(raw_str.substr(0, pos));
									raw_str.erase(0, pos + 1);

									pos = raw_str.find(',');
									read_vals[1] = std::stof(raw_str.substr(0, pos));
									raw_str.erase(0, pos + 1);

									pos = raw_str.find(',');
									read_vals[2] = std::stof(raw_str.substr(0, pos));
									raw_str.erase(0, pos + 1);

									read_vals[3] = std::stof(raw_str);
								}
								catch (...) {
									valid_reading = false;
								}
								if (valid_reading) {
									OSVR_Quaternion q;
									osvrQuatSetX(&q, read_vals[0]);
									osvrQuatSetY(&q, read_vals[1]);
									osvrQuatSetZ(&q, read_vals[2]);
									osvrQuatSetW(&q, read_vals[3]);
									relativ_rotation.store(q, std::memory_order_relaxed);
								}
							}
						}
						else if (raw_str[0] == 'C') {
							if (raw_str.size() > 3) {
								std::string raw_str_c = raw_str.substr(2, raw_str.size()); // Remove "C:"
								add_msg("Calibration: " + raw_str_c);
							}
						}
						else if (raw_str[0] == 'D') {
							if (raw_str.size() > 3) {
								std::string raw_str_d = raw_str.substr(2, raw_str.size()); // Remove "D:"
								add_msg("Info: " + raw_str_d);
							}
						}
						else {
							// invalid response...
						}
					}
				}

			}
			catch (...) {
				if (was_connected == true) {
					was_connected = false;
					add_msg("Connection with arduino lost, attempting to reconnect...");
				}
			}
		}
	}

	class Relativ_Device {
	public:
		Relativ_Device(OSVR_PluginRegContext ctx) {
			m_ctx = ctx;
			OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);
			osvrDeviceTrackerConfigure(opts, &m_tracker);
			m_dev.initAsync(ctx, DEVICE_NAME, opts);
			m_dev.sendJsonDescriptor(Relativ_OSVR_Plugin);
			m_dev.registerUpdateCallback(this);

			osvrQuatSetIdentity(&rotation_offset);
		}

		OSVR_ReturnCode update() {
			std::string next_msg;
			while (get_last_msg(&next_msg)) {
				osvr::pluginkit::log(m_ctx, OSVR_LOGLEVEL_NOTICE, next_msg.c_str());
			}
			OSVR_Quaternion ard_quat = relativ_rotation.load(std::memory_order_relaxed);
			tracker_rotation = get_osvr_quat_from_arduino_quat(ard_quat);

			if (GetAsyncKeyState(VK_SHIFT) != 0 && GetAsyncKeyState(VK_CONTROL) != 0 && GetAsyncKeyState(0x4F) != 0) {
				rotation_offset = quatConjugate(tracker_rotation);
			}

			tracker_rotation = quatMultiply(rotation_offset, tracker_rotation);
			osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &tracker_rotation, 0);
			return OSVR_RETURN_SUCCESS;
		}

		~Relativ_Device() {
			keep_thread_open = false;
		}

	private:
		osvr::pluginkit::DeviceToken m_dev;
		OSVR_TrackerDeviceInterface m_tracker;
		OSVR_Quaternion tracker_rotation;
		OSVR_Quaternion rotation_offset;
		OSVR_PluginRegContext m_ctx;

		struct AxisAngle { float a, x, y, z; };

		OSVR_Quaternion quatConjugate(OSVR_Quaternion a) {
			OSVR_Quaternion r;
			osvrQuatSetW(&r, osvrQuatGetW(&a));
			osvrQuatSetX(&r, -osvrQuatGetX(&a));
			osvrQuatSetY(&r, -osvrQuatGetY(&a));
			osvrQuatSetZ(&r, -osvrQuatGetZ(&a));

			return r;
		}

		OSVR_Quaternion quatGetYawOffset(OSVR_Quaternion a) {
			OSVR_Quaternion r;
			float qw = osvrQuatGetW(&a);
			float qx = osvrQuatGetX(&a);
			float qy = osvrQuatGetY(&a);
			float qz = osvrQuatGetZ(&a);

			double ysqr = qy * qy;

			// roll (x-axis rotation)
			double t0 = +2.0 * (qw * qx + qy * qz);
			double t1 = +1.0 - 2.0 * (qx * qx + ysqr);
			float roll = std::atan2(t0, t1);

			// pitch (y-axis rotation)
			double t2 = +2.0 * (qw * qy - qz * qx);
			t2 = t2 > 1.0 ? 1.0 : t2;
			t2 = t2 < -1.0 ? -1.0 : t2;
			float pitch = std::asin(t2);

			// yaw (z-axis rotation)
			double t3 = +2.0 * (qw * qz + qx * qy);
			double t4 = +1.0 - 2.0 * (ysqr + qz * qz);
			float yaw = std::atan2(t3, t4);


			t0 = std::cos(roll * 0.5);
			t1 = std::sin(roll * 0.5);
			t2 = std::cos(yaw * 0.5);
			t3 = std::sin(yaw * 0.5);
			t4 = std::cos(pitch * 0.5);
			double t5 = std::sin(pitch * 0.5);

			osvrQuatSetW(&r, t0 * t2 * t4 + t1 * t3 * t5);
			osvrQuatSetX(&r, t0 * t3 * t4 - t1 * t2 * t5);
			osvrQuatSetY(&r, t0 * t2 * t5 + t1 * t3 * t4);
			osvrQuatSetZ(&r, t1 * t2 * t4 - t0 * t3 * t5);
			return r;
		}

		OSVR_Quaternion quatMultiply(OSVR_Quaternion a, OSVR_Quaternion b) {
			OSVR_Quaternion r;
			float q1x = osvrQuatGetX(&a);
			float q1y = osvrQuatGetY(&a);
			float q1z = osvrQuatGetZ(&a);
			float q1w = osvrQuatGetW(&a);

			float q2x = osvrQuatGetX(&b);
			float q2y = osvrQuatGetY(&b);
			float q2z = osvrQuatGetZ(&b);
			float q2w = osvrQuatGetW(&b);

			osvrQuatSetX(&r, q1x * q2w + q1y * q2z - q1z * q2y + q1w * q2x);
			osvrQuatSetY(&r, -q1x * q2z + q1y * q2w + q1z * q2x + q1w * q2y);
			osvrQuatSetZ(&r, q1x * q2y - q1y * q2x + q1z * q2w + q1w * q2z);
			osvrQuatSetW(&r, -q1x * q2x - q1y * q2y - q1z * q2z + q1w * q2w);
			return r;
		}

		OSVR_Quaternion get_osvr_quat_from_arduino_quat(OSVR_Quaternion q) {
			AxisAngle aa = quat2AAngle(q);
			return aAngle2Quat(aa.a, -aa.x, aa.z, aa.y);
		}

		OSVR_Quaternion quatNormalize(OSVR_Quaternion &original) {
			float a, b, c, d;
			a = osvrQuatGetW(&original);
			b = osvrQuatGetX(&original);
			c = osvrQuatGetY(&original);
			d = osvrQuatGetZ(&original);
			double factor = std::sin(a / 2.0);
			// Calculate the x, y and z of the quaternion
			double x = b * factor;
			double y = c * factor;
			double z = d * factor;
			// Calculate the w value by cos( theta / 2 )
			double w = cos(a / 2.0);
			float len = std::sqrt(w*w + x*x + y*y + z*z);
			OSVR_Quaternion normalized = { w, x, y, z };
			return normalized;
		}

		AxisAngle quat2AAngle(OSVR_Quaternion &original) {
			OSVR_Quaternion inner = original;
			if (osvrQuatGetW(&inner) > 1) { // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
				inner = quatNormalize(inner);
			}
			float angle = 2 * std::acosf(osvrQuatGetW(&inner));
			float s = std::sqrt(1 - osvrQuatGetW(&inner)*osvrQuatGetW(&inner)); // assuming quaternion normalised then w is less than 1, so term always positive.
			float x, y, z;
			if (s < 0.001) { // test to avoid divide by zero, s is always positive due to sqrt
							 // if s close to zero then direction of axis not important
				x = osvrQuatGetX(&inner); // if it is important that axis is normalised then replace with x=1; y=z=0;
				y = osvrQuatGetY(&inner);
				z = osvrQuatGetZ(&inner);
			}
			else {
				x = osvrQuatGetX(&inner) / s; // normalise axis
				y = osvrQuatGetY(&inner) / s;
				z = osvrQuatGetZ(&inner) / s;
			}
			return{ angle, x, y, z };
		}

		OSVR_Quaternion aAngle2Quat(float a1, float x1, float y1, float z1) {
			float w, x, y, z;
			x = x1 * std::sin(a1 / 2);
			y = y1 * std::sin(a1 / 2);
			z = z1 * std::sin(a1 / 2);
			w = std::cos(a1 / 2);
			OSVR_Quaternion newQuat;
			osvrQuatSetW(&newQuat, w);
			osvrQuatSetX(&newQuat, x);
			osvrQuatSetY(&newQuat, y);
			osvrQuatSetZ(&newQuat, z);

			return newQuat;
		}
	};

	class Relativ_Tracker_Constructor {
	public:
		OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char* params) {
			osvr::pluginkit::log(ctx, OSVR_LOGLEVEL_INFO, "Loading Config...");
			std::string com_port;
			Json::Value config_params;
			if (params) {
				Json::Reader reader;
				bool parse_result = reader.parse(params, config_params);
				if (!parse_result) {
					osvr::pluginkit::log(ctx, OSVR_LOGLEVEL_CRITICAL, " Invalid config, load failed!");
					return OSVR_RETURN_FAILURE;
				}
				com_port = config_params.get("port", "").asString();
				if (com_port.length() < 1) {
					osvr::pluginkit::log(ctx, OSVR_LOGLEVEL_WARN, "Please set the \"port\" option to specify which port the arduino is on. eg. \"port\":\"COM4\"");
					return OSVR_RETURN_SUCCESS;
				}
				std::string info = "Connecting to arduino on " + com_port;
				osvr::pluginkit::log(ctx, OSVR_LOGLEVEL_INFO, info.c_str());
			}
			else {
				// No config params so we assume the user has our plugin installed just isn't using it
				return OSVR_RETURN_SUCCESS;
			}

			relativ_thread = std::thread(relativ_connect, com_port);
			osvr::pluginkit::registerObjectForDeletion(ctx, new Relativ_Device(ctx));
			return OSVR_RETURN_SUCCESS;
		}
	};
}

OSVR_PLUGIN(Relativ_OSVR_Plugin) {
	osvr::pluginkit::PluginContext context(ctx);
	osvr::pluginkit::registerDriverInstantiationCallback(ctx, DEVICE_NAME, new Relativ_Tracker_Constructor);
	return OSVR_RETURN_SUCCESS;
}
