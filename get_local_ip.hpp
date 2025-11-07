#pragma once
#include <boost/asio.hpp>
#include <iostream>

using namespace std;
using namespace boost::asio;
using ip::tcp;

string get_local_ip(io_context& io) {
    // Lấy tên máy hiện tại (hostname)
    string hostname = ip::host_name();

    // Dùng resolver để tìm các địa chỉ IP của hostname
    tcp::resolver resolver(io);
    tcp::resolver::results_type endpoints = resolver.resolve(hostname, "");

    // Duyệt các địa chỉ trả về và chọn IPv4 đầu tiên
    for (const auto& e : endpoints) {
        auto addr = e.endpoint().address();
        if (addr.is_v4() && !addr.is_loopback()) {  // tránh 127.0.0.1
            return addr.to_string();
        }
    }

    // fallback: nếu không tìm được, trả về localhost
    return "127.0.0.1";
}