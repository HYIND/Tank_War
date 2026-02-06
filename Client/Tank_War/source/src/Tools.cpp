#include "Helper/Tools.h"
#include <chrono>
#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>
#include <locale>
#include <format>

std::string Tool::WStringToUTF8(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

std::wstring Tool::UTF8ToWString(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}


int64_t Tool::GetTimestampMilliseconds()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int64_t Tool::GetTimestampSecond()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

std::string Tool::GetFormatSecondStr(int64_t timestamp_seconds)
{
	auto tp = std::chrono::time_point_cast<std::chrono::seconds>(
		std::chrono::system_clock::from_time_t(
			static_cast<std::time_t>(timestamp_seconds)
		)
	);
	return std::format("{:%Y-%m-%d %H:%M:%S}", tp);
}

std::string Tool::GenerateSimpleUuid()
{
	// 获取当前时间戳（毫秒精度）
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	// 初始化随机数生成器
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<uint16_t> dis(0, 0xFFFF);

	// 分解时间戳
	uint32_t time_low = static_cast<uint32_t>(millis & 0xFFFFFFFF);     // 低32位
	uint16_t time_mid = static_cast<uint16_t>((millis >> 32) & 0xFFFF); // 中16位
	uint16_t time_hi = static_cast<uint16_t>((millis >> 48) & 0x0FFF);  // 高12位

	// 生成4位随机数 (16位)
	uint16_t rand_num = dis(gen) & 0xFFFF;

	// 组合成UUID格式
	std::stringstream ss;
	ss << std::hex << std::setfill('0')
		<< std::setw(8) << time_low                         /* << "-" */
		<< std::setw(4) << time_mid                         /* << "-" */
		<< std::setw(4) << time_hi                          /* << "-" */
		<< std::setw(4) << (rand_num & 0x0FFF) /* << "-" */ // 使用12位随机数
		<< std::setw(4) << (rand_num >> 4);                 // 使用剩余4位

	return ss.str();
}

json Tool::ParseJson(const Buffer& buf)
{
	json result;
	std::string js_str(buf.Byte(), buf.Length());
	try
	{
		result = json::parse(js_str);
	}
	catch (...)
	{
		std::cout << "Tool ParseJson error : " << js_str << "\n";
	}
	return result;
}

float Tool::RadianToAngle(float radian)
{
	return radian / M_PI * 180.f;
}

float Tool::AngleToRadian(float angle)
{
	return angle / 180.f * M_PI;
}