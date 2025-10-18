#include <pybind11/pybind11.h>

namespace py = pybind11;

static float get_value(float sensor, float target)
{
    static float last_value = 50;
    const auto delta = (target - sensor) * 0.5f;
    if (delta > 0)
        last_value += copysignf(powf(fabs(delta) / 100, 1.5f), delta) * 100;
    else
        last_value += delta;
    last_value = std::clamp(last_value, 0.f, 100.f);
    return last_value;
}

PYBIND11_MODULE(pid_tuning, m, py::mod_gil_not_used()) {
    m.def("get_output", &get_value, py::arg("sensor"), py::arg("target"));
}
