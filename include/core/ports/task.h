#pragma once

// pull in the real executor‐alias and awaitable template
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>

// now you can alias away the details:
template<typename T>
using async_task = boost::asio::awaitable<T>;
