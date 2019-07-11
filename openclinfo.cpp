#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <CL/opencl.h>

void print_platform_info(cl_platform_id platform)
{
  static const std::vector<std::pair<cl_platform_info, std::string>> parameters {
    { CL_PLATFORM_PROFILE, "profile" },
    { CL_PLATFORM_VERSION, "version" },
    { CL_PLATFORM_NAME, "name" },
    { CL_PLATFORM_VENDOR, "vendor" },
    { CL_PLATFORM_EXTENSIONS, "extensions" }
  };

  static constexpr std::size_t MAX_TEXT_BUFFER_SIZE = 1024;
  std::size_t text_buffer_size;
  char text_buffer[MAX_TEXT_BUFFER_SIZE];

  for (const auto& p : parameters) {
    clGetPlatformInfo(platform, p.first, MAX_TEXT_BUFFER_SIZE, text_buffer, &text_buffer_size);
    std::cout << p.second << ": " << text_buffer << std::endl;
  }
}

template<typename T> T noop(const T& t) { return t; }
std::string bool_to_string(const cl_bool t) { return t ? "true" : "false"; }
std::string device_type_to_string(const cl_device_type t)
{
  std::ostringstream ss;
  if (t & CL_DEVICE_TYPE_CPU) { ss << "cpu"; }
  if (t & CL_DEVICE_TYPE_GPU) { ss << "gpu"; }
  if (t & CL_DEVICE_TYPE_ACCELERATOR) { ss << " accelerator"; }
  if (t & CL_DEVICE_TYPE_DEFAULT) { ss << " default"; }
  return ss.str();
}

std::string device_mem_cache_type_to_string(const cl_device_mem_cache_type t)
{
  switch (t) {
    case CL_NONE: return "none";
    case CL_READ_ONLY_CACHE: return "read only";
    case CL_READ_WRITE_CACHE: return "read write";
    default: return "FAIL";
  }
}

template<typename T, typename F>
void print_device_info(cl_device_id device, cl_device_info parameter, const char* label, F&& f)
{
  T t;
  const cl_int error = clGetDeviceInfo(device, parameter, sizeof(T), &t, nullptr);
  if (error != CL_SUCCESS) {
    std::cerr << "Failed to get info " << parameter << ": " << error << std::endl;
  } else {
    std::cout << label << ": " << f(t) << std::endl;
  }
}

void print_device_info(cl_device_id device)
{
  static const std::vector<std::pair<cl_device_info, std::string>> parameters {
    { CL_DEVICE_BUILT_IN_KERNELS, "built-in kernels" },
    { CL_DEVICE_EXTENSIONS, "extensions" },
    { CL_DEVICE_NAME, "device name" },
    { CL_DEVICE_OPENCL_C_VERSION, "opencl c version" },
    { CL_DEVICE_PROFILE, "profile" },
    { CL_DEVICE_VENDOR, "vendor" },
    { CL_DEVICE_VERSION, "version" },
    { CL_DRIVER_VERSION, "driver version" },
  };

  static constexpr std::size_t MAX_TEXT_BUFFER_SIZE = 1024;
  std::size_t text_buffer_size;
  char text_buffer[MAX_TEXT_BUFFER_SIZE];

  for (const auto& p : parameters) {
    clGetDeviceInfo(device, p.first,
                                   MAX_TEXT_BUFFER_SIZE, text_buffer, &text_buffer_size);
    std::cout << p.second << ": " << text_buffer << std::endl;
  }

  print_device_info<cl_device_type>(device,
                                    CL_DEVICE_TYPE,
                                    "type",
                                    device_type_to_string);

  print_device_info<cl_bool>(device,
                             CL_DEVICE_COMPILER_AVAILABLE,
                             "compiler available",
                             bool_to_string);

  print_device_info<cl_bool>(device,
                             CL_DEVICE_ENDIAN_LITTLE,
                             "little endian",
                             bool_to_string);

  print_device_info<cl_ulong>(device,
                              CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
                              "global cache in bytes",
                              noop<cl_ulong>);

  print_device_info<cl_device_mem_cache_type>(device,
                                              CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
                                              "global cache type",
                                              device_mem_cache_type_to_string);

  print_device_info<cl_bool>(device,
                             CL_DEVICE_IMAGE_SUPPORT,
                             "image support",
                             bool_to_string);
  
  // TODO there are many more fields ...

}

int main()
{
  static constexpr std::size_t MAX_PLATFORMS = 256;
  cl_platform_id platforms[MAX_PLATFORMS];
  cl_uint num_platforms;
  const cl_int error = clGetPlatformIDs(MAX_PLATFORMS, platforms, &num_platforms);
  if (num_platforms == MAX_PLATFORMS) {
    std::cerr << "maximum number of platforms exhausted." << std::endl;
  }

  if (error != CL_SUCCESS) {
    std::cerr << "Failed to get platform: " << error << std::endl;
    return false;
  }
  std::cout << "Found " << num_platforms << " platforms." << std::endl;
  for (std::size_t i = 0; i < num_platforms; ++i) {
    std::cout << "======================" << std::endl;
    std::cout << "Platform #" << i << ":" << std::endl;
    print_platform_info(platforms[i]);

    static constexpr std::size_t MAX_DEVICES = 256;
    cl_device_id devices[MAX_DEVICES];
    cl_uint num_devices;
    const cl_int error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL,
                                                       MAX_DEVICES, devices, &num_devices);
    if (num_devices == MAX_DEVICES) {
      std::cerr << "maximum number of devices exhausted." << std::endl;
    }

    if (error != CL_SUCCESS) {
      std::cerr << "Failed to get device: " << error << std::endl;
      return false;
    }

    std::cout << "number of devices: " << num_devices << std::endl;
    std::cout << "--------------------" << std::endl;
    for (std::size_t j = 0; j < num_devices; ++j) {
      std::cout << "Device #" << i << std::endl;
      print_device_info(devices[j]);
    }
  }
}

