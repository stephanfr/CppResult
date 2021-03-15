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
        ResultBase(BaseResultCodes success_or_failure, const std::string message)
            : success_or_failure_(success_or_failure), message_(message)
        {
        }

        ResultBase(BaseResultCodes success_or_failure, const ResultBase& inner_error, const std::string message)
            : success_or_failure_(success_or_failure), message_(message), inner_error_(inner_error.shallow_copy())
        {
        }

       public:
        virtual ~ResultBase(){};

        virtual std::unique_ptr<const ResultBase> shallow_copy() const = 0;

        bool succeeded() const { return (success_or_failure_ == BaseResultCodes::SUCCESS); }

        bool failed() const { return (success_or_failure_ == BaseResultCodes::FAILURE); }

        const std::string message() const { return (message_); }

        const std::unique_ptr<const ResultBase>& inner_error() const { return (inner_error_); }

        virtual const std::type_info& error_code_type() const = 0;
        virtual int error_code_value() const = 0;

       protected:
        BaseResultCodes success_or_failure_;

        std::string message_;

        std::unique_ptr<const ResultBase> inner_error_;
    };

    template <typename TErrorCodeEnum>
    class Result : public ResultBase
    {
       protected:
        typedef TErrorCodeEnum ErrorCodeType;

        Result(BaseResultCodes success_or_failure, TErrorCodeEnum error_code, const std::string message)
            : ResultBase(success_or_failure, message), error_code_(error_code)
        {
            assert((success_or_failure == BaseResultCodes::SUCCESS) ||
                   ((success_or_failure == BaseResultCodes::FAILURE) && (error_code != TErrorCodeEnum::SUCCESS)));
        }

        template <typename TInnerErrorCodeEnum>
        Result(BaseResultCodes success_or_failure, const Result<TInnerErrorCodeEnum>& inner_error,
               TErrorCodeEnum error_code, const std::string message)
            : ResultBase(success_or_failure, inner_error, message), error_code_(error_code)
        {
            assert((success_or_failure == BaseResultCodes::SUCCESS) ||
                   ((success_or_failure == BaseResultCodes::FAILURE) && (error_code != TErrorCodeEnum::SUCCESS)));
        }

        std::unique_ptr<const ResultBase> shallow_copy() const
        {
            return (std::unique_ptr<const ResultBase>(new Result<TErrorCodeEnum>(*this)));
        }

       public:
        Result(const Result<TErrorCodeEnum>& result_to_copy)
            : ResultBase(result_to_copy.success_or_failure_, result_to_copy.message_), error_code_(result_to_copy.error_code_)
        {
            inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);
        }

        virtual ~Result(){};

        const Result<TErrorCodeEnum>& operator=(const Result<TErrorCodeEnum>& result_to_copy)
        {
            success_or_failure_ = result_to_copy.success_or_failure_;
            message_ = result_to_copy.message_;
            error_code_ = result_to_copy.error_code_;

            inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);

            return (*this);
        }

        static Result<TErrorCodeEnum> success()
        {
            return (Result(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"));
        };

        static Result<TErrorCodeEnum> failure(TErrorCodeEnum error_code, const std::string& message)
        {
            return (Result(BaseResultCodes::FAILURE, error_code, message));
        }

        template <typename... Args>
        static Result<TErrorCodeEnum> failure(TErrorCodeEnum error_code, const std::string& format, Args... args)
        {
            return (Result(BaseResultCodes::FAILURE, error_code, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static Result<TErrorCodeEnum> failure(const Result<TInnerErrorCodeEnum>& inner_error, TErrorCodeEnum error_code,
                                              const std::string& message)
        {
            return (Result(BaseResultCodes::FAILURE, inner_error, error_code, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static Result<TErrorCodeEnum> failure(const Result<TInnerErrorCodeEnum>& inner_error, TErrorCodeEnum error_code,
                                              const std::string& format, Args... args)
        {
            return (Result(BaseResultCodes::FAILURE, inner_error, error_code, fmt::format(format, args...)));
        }

        TErrorCodeEnum error_code() const { return (error_code_); }

        const std::type_info& error_code_type() const { return (typeid(ErrorCodeType)); }

        int error_code_value() const { return ((int)error_code_); }

       protected:
        TErrorCodeEnum error_code_;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnValue : public Result<TErrorCodeEnum>
    {
       protected:
        ResultWithReturnValue(BaseResultCodes success_or_failure, TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, error_code, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnValue(BaseResultCodes success_or_failure, const Result<TInnerErrorCodeEnum>& inner_error,
                              TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, inner_error, error_code, message)
        {
        }

       public:
        ResultWithReturnValue(TResultType return_value)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              return_value_(return_value)
        {
        }

        ResultWithReturnValue(const ResultWithReturnValue& result_to_copy)
            : Result<TErrorCodeEnum>(result_to_copy.success_or_failure_, result_to_copy.error_code_, result_to_copy.message_),
              return_value_(result_to_copy.return_value_)
        {
            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);
        }

        virtual ~ResultWithReturnValue(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->success_or_failure_, this->error_code_, this->message_));
        }

        const ResultWithReturnValue<TErrorCodeEnum, TResultType>& operator=(
            const ResultWithReturnValue<TErrorCodeEnum, TResultType>& result_to_copy)
        {
            ResultBase::success_or_failure_ = result_to_copy.success_or_failure_;
            ResultBase::message_ = result_to_copy.message_;
            Result<TErrorCodeEnum>::error_code_ = result_to_copy.error_code_;
            return_value_ = result_to_copy.return_value_;

            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);

            return (*this);
        }

        static ResultWithReturnValue<TErrorCodeEnum, TResultType> success(const TResultType& return_value)
        {
            return (ResultWithReturnValue(return_value));
        };

        static ResultWithReturnValue<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                          const std::string& message)
        {
            return (ResultWithReturnValue(BaseResultCodes::FAILURE, error_code, message));
        }

        template <typename... Args>
        static ResultWithReturnValue<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                          const std::string& format, Args... args)
        {
            return (ResultWithReturnValue(BaseResultCodes::FAILURE, error_code, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnValue<TErrorCodeEnum, TResultType> failure(const Result<TInnerErrorCodeEnum>& inner_error,
                                                                          TErrorCodeEnum error_code,
                                                                          const std::string& message)
        {
            return (ResultWithReturnValue(BaseResultCodes::FAILURE, inner_error, error_code, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnValue<TErrorCodeEnum, TResultType> failure(const Result<TInnerErrorCodeEnum>& inner_error,
                                                                          TErrorCodeEnum error_code,
                                                                          const std::string& format, Args... args)
        {
            return (
                ResultWithReturnValue(BaseResultCodes::FAILURE, inner_error, error_code, fmt::format(format, args...)));
        }

        TResultType& return_value()
        {
            assert(return_value_);
            return (*return_value_);
        }

       protected:
        std::optional<TResultType> return_value_;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnRef : public Result<TErrorCodeEnum>
    {
       protected:
        ResultWithReturnRef(BaseResultCodes success_or_failure, TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, error_code, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnRef(BaseResultCodes success_or_failure, const Result<TInnerErrorCodeEnum>& inner_error,
                            TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, inner_error, error_code, message)
        {
        }

       public:
        ResultWithReturnRef(TResultType& return_ref)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              return_ref_(return_ref)
        {
        }

        ResultWithReturnRef(const ResultWithReturnRef& result_to_copy)
            : Result<TErrorCodeEnum>(result_to_copy.success_or_failure_, result_to_copy.error_code_, result_to_copy.message_),
              return_ref_(result_to_copy.return_ref_)
        {
            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);
        }

        virtual ~ResultWithReturnRef(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->success_or_failure_, this->error_code_, this->message_));
        }

        const ResultWithReturnRef<TErrorCodeEnum, TResultType>& operator=(
            const ResultWithReturnRef<TErrorCodeEnum, TResultType>& result_to_copy)
        {
            ResultBase::success_or_failure_ = result_to_copy.success_or_failure_;
            ResultBase::message_ = result_to_copy.message_;
            Result<TErrorCodeEnum>::error_code_ = result_to_copy.error_code_;
            return_ref_ = result_to_copy.return_ref_;

            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);

            return (*this);
        }

        static ResultWithReturnRef<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                        const std::string& message)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, error_code, message));
        }

        template <typename... Args>
        static ResultWithReturnRef<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                        const std::string& format, Args... args)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, error_code, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnRef<TErrorCodeEnum, TResultType> failure(const Result<TInnerErrorCodeEnum>& inner_error,
                                                                        TErrorCodeEnum error_code,
                                                                        const std::string& message)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, inner_error, error_code, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnRef<TErrorCodeEnum, TResultType> failure(const Result<TInnerErrorCodeEnum>& inner_error,
                                                                        TErrorCodeEnum error_code,
                                                                        const std::string& format, Args... args)
        {
            return (ResultWithReturnRef(BaseResultCodes::FAILURE, inner_error, error_code, fmt::format(format, args...)));
        }

        TResultType& ReturnRef()
        {
            assert(return_ref_);
            return (*return_ref_);
        }

       protected:
        std::optional<std::reference_wrapper<TResultType>> return_ref_;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnUniquePtr : public Result<TErrorCodeEnum>
    {
       private:
        ResultWithReturnUniquePtr(BaseResultCodes success_or_failure, TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, error_code, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnUniquePtr(BaseResultCodes success_or_failure, const Result<TInnerErrorCodeEnum>& inner_error,
                                  TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, inner_error, error_code, message)
        {
        }

       public:
        explicit ResultWithReturnUniquePtr(std::unique_ptr<TResultType>& return_ptr)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              return_ptr_(std::move(return_ptr))
        {
        }

        ResultWithReturnUniquePtr(ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType>& result_to_copy)
            : Result<TErrorCodeEnum>(result_to_copy.success_or_failure_, result_to_copy.error_code_, result_to_copy.message_),
              return_ptr_(std::move(result_to_copy.return_ptr_))
        {
            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);
        }

        virtual ~ResultWithReturnUniquePtr(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->success_or_failure_, this->error_code_, this->message_));
        }

        const ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType>& operator=(
                const ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType>& result_to_copy) = delete;


        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> success(std::unique_ptr<TResultType>& return_value)
        {
            return (ResultWithReturnUniquePtr(return_value));
        }

        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                              const std::string& message)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, error_code, message));
        }

        template <typename... Args>
        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                              const std::string& format, Args... args)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, error_code, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> failure(
            const Result<TInnerErrorCodeEnum>& inner_error, TErrorCodeEnum error_code, const std::string& message)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, inner_error, error_code, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnUniquePtr<TErrorCodeEnum, TResultType> failure(
            const Result<TInnerErrorCodeEnum>& inner_error, TErrorCodeEnum error_code, const std::string& format,
            Args... args)
        {
            return (ResultWithReturnUniquePtr(BaseResultCodes::FAILURE, inner_error, error_code,
                                              fmt::format(format, args...)));
        }

        std::unique_ptr<TResultType>& return_ptr() { return (return_ptr_); }

       private:
        std::unique_ptr<TResultType> return_ptr_;
    };

    template <typename TErrorCodeEnum, typename TResultType>
    class ResultWithReturnSharedPtr : public Result<TErrorCodeEnum>
    {
       private:
        ResultWithReturnSharedPtr(BaseResultCodes success_or_failure, TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, error_code, message)
        {
        }

        template <typename TInnerErrorCodeEnum>
        ResultWithReturnSharedPtr(BaseResultCodes success_or_failure, const Result<TInnerErrorCodeEnum>& inner_error,
                                  TErrorCodeEnum error_code, const std::string message)
            : Result<TErrorCodeEnum>(success_or_failure, inner_error, error_code, message)
        {
        }

       public:
        ResultWithReturnSharedPtr(std::shared_ptr<TResultType>& return_ptr)
            : Result<TErrorCodeEnum>(BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success"),
              return_ptr_(return_ptr)
        {
        }

        ResultWithReturnSharedPtr(const ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType>& result_to_copy)
            : Result<TErrorCodeEnum>(result_to_copy.success_or_failure_, result_to_copy.error_code_, result_to_copy.message_),
              return_ptr_(result_to_copy.return_ptr_)
        {
            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);
        }

        virtual ~ResultWithReturnSharedPtr(){};

        operator const Result<TErrorCodeEnum>&() const
        {
            return (Result<TErrorCodeEnum>(this->success_or_failure_, this->error_code_, this->message_));
        }

        const ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType>& operator=(
            const ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType>& result_to_copy)
        {
            ResultBase::success_or_failure_ = result_to_copy.success_or_failure_;
            ResultBase::message_ = result_to_copy.message_;
            Result<TErrorCodeEnum>::error_code_ = result_to_copy.error_code_;
            return_ptr_ = result_to_copy.return_ptr_;

            this->inner_error_ = (result_to_copy.inner_error_ ? result_to_copy.inner_error_->shallow_copy() : nullptr);

            return (*this);
        }

        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> success(
            std::shared_ptr<TResultType>& return_value) = delete;

        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                              const std::string& message)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, error_code, message));
        }

        template <typename... Args>
        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> failure(TErrorCodeEnum error_code,
                                                                              const std::string& format, Args... args)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, error_code, fmt::format(format, args...)));
        }

        template <typename TInnerErrorCodeEnum>
        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> failure(
            const Result<TInnerErrorCodeEnum>& inner_error, TErrorCodeEnum error_code, const std::string& message)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, inner_error, error_code, message));
        }

        template <typename TInnerErrorCodeEnum, typename... Args>
        static ResultWithReturnSharedPtr<TErrorCodeEnum, TResultType> failure(
            const Result<TInnerErrorCodeEnum>& inner_error, TErrorCodeEnum error_code, const std::string& format,
            Args... args)
        {
            return (ResultWithReturnSharedPtr(BaseResultCodes::FAILURE, inner_error, error_code,
                                              fmt::format(format, args...)));
        }

        std::shared_ptr<TResultType>& return_ptr() { return (return_ptr_); }

       private:
        std::shared_ptr<TResultType> return_ptr_;
    };

}  // namespace SEFUtility