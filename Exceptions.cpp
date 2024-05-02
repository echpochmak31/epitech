#include "stdexcept"

#pragma once

namespace nts
{
    class NanoTekSpiceException : public std::exception {
    private:
        std::string message;

    public:
        explicit NanoTekSpiceException(const char* message) :  std::exception(), message(message) {}

        virtual const char* what() const noexcept override {
            return message.c_str();
        }
    };

    class InvalidSyntaxException : public NanoTekSpiceException {
    public:
        explicit InvalidSyntaxException(const char* message) : NanoTekSpiceException(message) {}
    };

    class UnknownComponentTypeException : public NanoTekSpiceException {
    public:
        explicit UnknownComponentTypeException(const char* message) : NanoTekSpiceException(message) {}
    };

    class UnknownComponentNameException : public NanoTekSpiceException {
    public:
        explicit UnknownComponentNameException(const char* message) : NanoTekSpiceException(message) {}
    };

    class NonUniqueComponentNameException : public NanoTekSpiceException {
    public:
        explicit NonUniqueComponentNameException(const char* message) : NanoTekSpiceException(message) {}
    };

    class NoChipsetsException : public NanoTekSpiceException {
    public:
        explicit NoChipsetsException(const char* message) : NanoTekSpiceException(message) {}
    };

    class InvalidOperationException : public NanoTekSpiceException {
    public:
        explicit InvalidOperationException(const char* message) : NanoTekSpiceException(message) {}
    };
}

