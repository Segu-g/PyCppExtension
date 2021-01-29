#pragma once
#include <Python.h>
#include <exception>
#include <string>

namespace mdl {
    class ExtException: public std::exception {
    public:
        ExtException();
        ExtException(PyObject* exception);
        ExtException(PyObject* exception, std::string text);
        ExtException(PyObject* exception, PyObject* value);
        ~ExtException() override;
        const char* what();
        void set_err() ;
    private:
        PyObject* exception;
        PyObject* value;
    };
}
