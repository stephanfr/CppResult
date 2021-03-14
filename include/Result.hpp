#pragma once

#include <assert.h>
#include <memory>
#include <optional>
#include <string>

#include <fmt/format.h>

namespace SEFUtility
{
    enum class BaseResultCodes
    {
        SUCCESS = 0,
        FAILURE
    };

    //
    //	Base Class needed primarily for passing inner errors
    //

    class ResultBase
    {
       protected:
        ResultBase(BaseResultCodes successOrFailure, const std::string message)
            : m_successOrFailure(successOrFailure), m_message(message)
        {
        }

        ResultBase(BaseResultCodes successOrFailure, const ResultBase& innerError, const std::string message)
            : m_successOrFailure(successOrFailure), m_message(message), m_innerError(innerError.shallowCopy())
        {
        }

       public:
        virtual ~ResultBase(){};

        virtual std::unique_ptr<const ResultBase> shallowCopy() const = 0;

        bool Succeeded() const { return (m_successOrFailure == BaseResultCodes::SUCCESS); }

        bool Failed() const { return (m_successOrFailure == BaseResultCodes::FAILURE); }

        const std::string message() const { return (m_message); }

        const std::unique_ptr<const ResultBase>& innerError() const { return (m_innerError); }

        virtual const std::type_info& errorCodeType() const = 0;
        virtual int errorCodeValue() const = 0;

       protected:
        BaseResultCodes m_successOrFailure;

        std::string m_message;

        std::unique_ptr<const ResultBase> m_innerError;
    };

    template <typename TErrorCodeEnum>
    class Result : public ResultBase
    {
       protected:
        typedef TErrorCodeEnum ErrorCodeType;

        Result(BaseResultCodes successOrFailure, TErrorCodeEnum errorCode, const std::string message)
            : ResultBase(successOrFailure, message), m_errorCode(errorCode)
        {
            assert((successOrFailure == BaseResultCodes::SUCCESS) ||
                   ((successOrFailure == BaseResultCodes::FAILURE) && (errorCode != TErrorCodeEnum::SUCCESS)));
        }

        template <typename TInnerErrorCodeEnum>
        Result(BaseResultCodes successOrFailure, const Result<TInnerErrorCodeEnum>& innerError,
               TErrorCodeEnum errorCode, const std::string message)
            : ResultBase(successOrFailure, innerError, message), m_errorCode(errorCode)
        {
            assert((successOrFailure == BaseResultCodes::SUCCESS) ||
                   ((successOrFailure == BaseResultCodes::FAILURE) && (errorCode != TErrorCodeEnum::SUCCESS)));
        }

        std::unique_ptr<const ResultBase> shallowCopy() const
        {
            return (std::unique_ptr<const ResultBase>(new Result<TErrorCodeEnum>(*this)));
        }

       public:
        Result(const Result<TErrorCodeEnum>& resultToCopy)
            : ResultBase(resultToCopy.m_successOrFailure, resultToCopy.m_message), m_errorCode(resultToCopy.m_errorCode)
        {
            m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);
        }

        virtual ~Result(){};

        const Result<TErrorCodeEnum>& operator=(const Result<TErrorCodeEnum>& resultToCopy)
        {
            m_successOrFailure = resultToCopy.m_successOrFailure;
            m_message = resultToCopy.m_message;
            m_errorCode = resultToCopy.m_errorCode;

            m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);

            return (*this);
        }

        static Result<TErrorCodeEnum> Success()
        {
            return (Result(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"));
        };

        static Result<TErrorCodeEnum> Failure(TErrorCodeEnum errorCode, const std::string& message)
        {
            return (Result(BaseResultCodes::FAILURE, errorCode, message));
        }

        template <typename... Args>
        static Result<TErrorCodeEnum> Failure(TErrorCodeEnum errorCode, const std::string& format, Args... args)
        {
            return (Result(BaseResultCodes::FAILURE, errorCode, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static Result<TErrorCodeEnum> Failure(const Result<TInnerErrorCodeEnum>& innerError, TErrorCodeEnum errorCode,
                                              const std::string& message)
        {
            return (Result(BaseResultCodes::FAILURE, innerError, errorCode, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static Result<TErrorCodeEnum> Failure(const Result<TInnerErrorCodeEnum>& innerError, TErrorCodeEnum errorCode,
                                              const std::string& format, Args... args)
        {
            return (Result(BaseResultCodes::FAILURE, innerError, errorCode, fmt::format(format, args...)));
        }

        TErrorCodeEnum errorCode() const { return (m_errorCode); }

        const std::type_info& errorCodeType() const { return (typeid(ErrorCodeType)); }

        int errorCodeValue() const { return ((int)m_errorCode); }

       protected:
        TErrorCodeEnum m_errorCode;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnValue : public Result<TErrorCodeEnum>
    {
       protected:
        ResultWithReturnValue(BaseResultCodes successOrFailure, TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, errorCode, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnValue(BaseResultCodes successOrFailure, const Result<TInnerErrorCodeEnum>& innerError,
                              TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, innerError, errorCode, message)
        {
        }

       public:
        ResultWithReturnValue(TResultType returnValue)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              m_returnValue(returnValue)
        {
        }

        ResultWithReturnValue(const ResultWithReturnValue& resultToCopy)
            : Result<TErrorCodeEnum>(resultToCopy.m_successOrFailure, resultToCopy.m_errorCode, resultToCopy.m_message),
              m_returnValue(resultToCopy.m_returnValue)
        {
            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);
        }

        virtual ~ResultWithReturnValue(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->m_successOrFailure, this->m_errorCode, this->m_message));
        }

        const ResultWithReturnValue<TErrorCodeEnum, TResultType>& operator=(
            const ResultWithReturnValue<TErrorCodeEnum, TResultType>& resultToCopy)
        {
            ResultBase::m_successOrFailure = resultToCopy.m_successOrFailure;
            ResultBase::m_message = resultToCopy.m_message;
            Result<TErrorCodeEnum>::m_errorCode = resultToCopy.m_errorCode;
            m_returnValue = resultToCopy.m_returnValue;

            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);

            return (*this);
        }

        static ResultWithReturnValue<TErrorCodeEnum, TResultType> Success(const TResultType& return_value)
        {
            return (ResultWithReturnValue(return_value));
        };

        static ResultWithReturnValue<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                          const std::string& message)
        {
            return (ResultWithReturnValue(BaseResultCodes::FAILURE, errorCode, message));
        }

        template <typename... Args>
        static ResultWithReturnValue<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                          const std::string& format, Args... args)
        {
            return (ResultWithReturnValue(BaseResultCodes::FAILURE, errorCode, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnValue<TErrorCodeEnum, TResultType> Failure(const Result<TInnerErrorCodeEnum>& innerError,
                                                                          TErrorCodeEnum errorCode,
                                                                          const std::string& message)
        {
            return (ResultWithReturnValue(BaseResultCodes::FAILURE, innerError, errorCode, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnValue<TErrorCodeEnum, TResultType> Failure(const Result<TInnerErrorCodeEnum>& innerError,
                                                                          TErrorCodeEnum errorCode,
                                                                          const std::string& format, Args... args)
        {
            return (
                ResultWithReturnValue(BaseResultCodes::FAILURE, innerError, errorCode, fmt::format(format, args...)));
        }

        TResultType& ReturnValue()
        {
            assert(m_returnValue);
            return (*m_returnValue);
        }

       protected:
        std::optional<TResultType> m_returnValue;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnRef : public Result<TErrorCodeEnum>
    {
       protected:
        ResultWithReturnRef(BaseResultCodes successOrFailure, TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, errorCode, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnRef(BaseResultCodes successOrFailure, const Result<TInnerErrorCodeEnum>& innerError,
                            TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, innerError, errorCode, message)
        {
        }

       public:
        ResultWithReturnRef(TResultType& returnRef)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              m_returnRef(returnRef)
        {
        }

        ResultWithReturnRef(const ResultWithReturnRef& resultToCopy)
            : Result<TErrorCodeEnum>(resultToCopy.m_successOrFailure, resultToCopy.m_errorCode, resultToCopy.m_message),
              m_returnRef(resultToCopy.m_returnRef)
        {
            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);
        }

        virtual ~ResultWithReturnRef(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->m_successOrFailure, this->m_errorCode, this->m_message));
        }

        const ResultWithReturnRef<TErrorCodeEnum, TResultType>& operator=(
            const ResultWithReturnRef<TErrorCodeEnum, TResultType>& resultToCopy)
        {
            ResultBase::m_successOrFailure = resultToCopy.m_successOrFailure;
            ResultBase::m_message = resultToCopy.m_message;
            Result<TErrorCodeEnum>::m_errorCode = resultToCopy.m_errorCode;
            m_returnRef = resultToCopy.m_returnRef;

            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);

            return (*this);
        }

        static ResultWithReturnRef<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                        const std::string& message)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, errorCode, message));
        }

        template <typename... Args>
        static ResultWithReturnRef<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                        const std::string& format, Args... args)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, errorCode, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnRef<TErrorCodeEnum, TResultType> Failure(const Result<TInnerErrorCodeEnum>& innerError,
                                                                        TErrorCodeEnum errorCode,
                                                                        const std::string& message)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, innerError, errorCode, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnRef<TErrorCodeEnum, TResultType> Failure(const Result<TInnerErrorCodeEnum>& innerError,
                                                                        TErrorCodeEnum errorCode,
                                                                        const std::string& format, Args... args)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, innerError, errorCode, fmt::format(format, args...)));
        }

        TResultType& ReturnRef()
        {
            assert(m_returnRef);
            return (*m_returnRef);
        }

       protected:
        std::optional<std::reference_wrapper<TResultType>> m_returnRef;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnUniquePtr : public Result<TErrorCodeEnum>
    {
       private:
        ResultWithReturnUniquePtr(BaseResultCodes successOrFailure, TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, errorCode, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnUniquePtr(BaseResultCodes successOrFailure, const Result<TInnerErrorCodeEnum>& innerError,
                                  TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, innerError, errorCode, message)
        {
        }

       public:
        explicit ResultWithReturnUniquePtr(std::unique_ptr<TResultType>& returnValue)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              m_returnPtr(std::move(returnValue))
        {
        }

        ResultWithReturnUniquePtr(ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType>& resultToCopy)
            : Result<TErrorCodeEnum>(resultToCopy.m_successOrFailure, resultToCopy.m_errorCode, resultToCopy.m_message),
              m_returnPtr(std::move(resultToCopy.m_returnPtr))
        {
            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);
        }

        virtual ~ResultWithReturnUniquePtr(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->m_successOrFailure, this->m_errorCode, this->m_message));
        }

        const ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType>& operator=(
                const ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType>& resultToCopy) = delete;


        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> Success(std::unique_ptr<TResultType>& returnValue)
        {
            return (ResultWithReturnUniquePtr(returnValue));
        }

        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                              const std::string& message)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, errorCode, message));
        }

        template <typename... Args>
        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                              const std::string& format, Args... args)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, errorCode, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> Failure(
            const Result<TInnerErrorCodeEnum>& innerError, TErrorCodeEnum errorCode, const std::string& message)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, innerError, errorCode, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> Failure(
            const Result<TInnerErrorCodeEnum>& innerError, TErrorCodeEnum errorCode, const std::string& format,
            Args... args)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, innerError, errorCode,
                                              fmt::format(format, args...)));
        }

        std::unique_ptr<TResultType>& ReturnPtr() { return (m_returnPtr); }

       private:
        std::unique_ptr<TResultType> m_returnPtr;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnSharedPtr : public Result<TErrorCodeEnum>
    {
       private:
        ResultWithReturnSharedPtr(BaseResultCodes successOrFailure, TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, errorCode, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnSharedPtr(BaseResultCodes successOrFailure, const Result<TInnerErrorCodeEnum>& innerError,
                                  TErrorCodeEnum errorCode, const std::string message)
            : Result<TErrorCodeEnum>(successOrFailure, innerError, errorCode, message)
        {
        }

       public:
        ResultWithReturnSharedPtr(std::shared_ptr<TResultType>& returnValue)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              m_returnPtr(returnValue)
        {
        }

        ResultWithReturnSharedPtr(const ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType>& resultToCopy)
            : Result<TErrorCodeEnum>(resultToCopy.m_successOrFailure, resultToCopy.m_errorCode, resultToCopy.m_message),
              m_returnPtr(resultToCopy.m_returnPtr)
        {
            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);
        }

        virtual ~ResultWithReturnSharedPtr(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->m_successOrFailure, this->m_errorCode, this->m_message));
        }

        const ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType>& operator=(
            const ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType>& resultToCopy)
        {
            ResultBase::m_successOrFailure = resultToCopy.m_successOrFailure;
            ResultBase::m_message = resultToCopy.m_message;
            Result<TErrorCodeEnum>::m_errorCode = resultToCopy.m_errorCode;
            m_returnPtr = resultToCopy.m_returnPtr;

            this->m_innerError = (resultToCopy.m_innerError ? resultToCopy.m_innerError->shallowCopy() : nullptr);

            return (*this);
        }

        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> Success(
            std::shared_ptr<TResultType>& returnValue) = delete;

        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                              const std::string& message)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, errorCode, message));
        }

        template <typename... Args>
        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> Failure(TErrorCodeEnum errorCode,
                                                                              const std::string& format, Args... args)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, errorCode, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> Failure(
            const Result<TInnerErrorCodeEnum>& innerError, TErrorCodeEnum errorCode, const std::string& message)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, innerError, errorCode, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> Failure(
            const Result<TInnerErrorCodeEnum>& innerError, TErrorCodeEnum errorCode, const std::string& format,
            Args... args)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, innerError, errorCode,
                                              fmt::format(format, args...)));
        }

        std::shared_ptr<TResultType>& ReturnPtr() { return (m_returnPtr); }

       private:
        std::shared_ptr<TResultType> m_returnPtr;
    };

}  // namespace SEFUtility