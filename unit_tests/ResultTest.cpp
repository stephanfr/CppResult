#include <catch2/catch_all.hpp>

#include "Result.hpp"

//  The pragma below is to disable to false errors flagged by intellisense for
//  Catch2 REQUIRE macros.

#if __INTELLISENSE__
#pragma diag_suppress 2486
#endif

using SEFUtility::Result;
using SEFUtility::ResultWithReturnRef;
using SEFUtility::ResultWithReturnSharedPtr;
using SEFUtility::ResultWithReturnUniquePtr;
using SEFUtility::ResultWithReturnValue;

enum class ErrorCodes1
{
    SUCCESS = 0,
    FAILURE_1 = 1000,
    FAILURE_2,
    FAILURE_3
};

enum class ErrorCodes2
{
    SUCCESS = 0,
    FAILURE_1 = 1000,
    FAILURE_2,
    FAILURE_3
};

TEST_CASE("Result Test", "[basic-checks]")
{
    auto testResult1 = Result<ErrorCodes1>::Success();

    REQUIRE(testResult1.Succeeded());
    REQUIRE(!testResult1.Failed());
    REQUIRE(testResult1.errorCode() == ErrorCodes1::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(!testResult1.innerError());

    auto testResult2 = Result<ErrorCodes1>::Failure(ErrorCodes1::FAILURE_1, "message");

    REQUIRE(testResult2.Failed());
    REQUIRE(!testResult2.Succeeded());
    REQUIRE(testResult2.errorCode() == ErrorCodes1::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.errorCodeValue() == 1000);
    REQUIRE(!testResult2.innerError());

    auto testResult3 = Result<ErrorCodes1>::Failure(ErrorCodes1::FAILURE_2, "message {} {}", "test 3", 35);

    REQUIRE(testResult3.Failed());
    REQUIRE(!testResult3.Succeeded());
    REQUIRE(testResult3.errorCode() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.errorCodeValue() == 1001);
    REQUIRE(!testResult3.innerError());

    auto testResult4 = Result<ErrorCodes1>::Failure(testResult3, ErrorCodes1::FAILURE_2, "message without var args");

    REQUIRE(testResult4.Failed());
    REQUIRE(!testResult4.Succeeded());
    REQUIRE(testResult4.errorCode() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult4.message() == "message without var args");
    REQUIRE(testResult4.errorCodeValue() == 1001);
    REQUIRE(testResult4.innerError());
    REQUIRE(testResult4.innerError()->Failed());
    REQUIRE(testResult4.innerError()->errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(testResult4.innerError()->message() == "message test 3 35");
    REQUIRE(testResult4.innerError()->errorCodeValue() == 1001);
    REQUIRE(!testResult4.innerError()->innerError());

    auto testResult5 =
        Result<ErrorCodes1>::Failure(testResult3, ErrorCodes1::FAILURE_3, "message {} {}", "test 4", 102);

    REQUIRE(testResult5.Failed());
    REQUIRE(!testResult5.Succeeded());
    REQUIRE(testResult5.errorCode() == ErrorCodes1::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.errorCodeValue() == 1002);
    REQUIRE(testResult5.innerError());
    REQUIRE(testResult5.innerError()->Failed());
    REQUIRE(testResult5.innerError()->errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(testResult5.innerError()->message() == "message test 3 35");
    REQUIRE(testResult5.innerError()->errorCodeValue() == 1001);
    REQUIRE(!testResult5.innerError()->innerError());
}

TEST_CASE("Result with Return Value Test", "[basic-checks]")
{
    std::string return_value("returned value");

    auto testResult1 = ResultWithReturnValue<ErrorCodes2, std::string>::Success(return_value);

    REQUIRE(testResult1.Succeeded());
    REQUIRE(!testResult1.Failed());
    REQUIRE(testResult1.errorCode() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.innerError());
    REQUIRE(testResult1.ReturnValue() == "returned value");
    return_value = "new value";
    REQUIRE(testResult1.ReturnValue() == "returned value");

    auto testResult2 = ResultWithReturnValue<ErrorCodes2, std::string>::Failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.Failed());
    REQUIRE(!testResult2.Succeeded());
    REQUIRE(testResult2.errorCode() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.errorCodeValue() == 1000);
    REQUIRE(!testResult2.innerError());

    auto testResult3 =
        ResultWithReturnValue<ErrorCodes1, std::string>::Failure(ErrorCodes1::FAILURE_2, "message {} {}", "test 3", 35);

    REQUIRE(testResult3.Failed());
    REQUIRE(!testResult3.Succeeded());
    REQUIRE(testResult3.errorCode() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.errorCodeValue() == 1001);
    REQUIRE(!testResult3.innerError());

    auto testResult4 =
        ResultWithReturnValue<ErrorCodes2, std::string>::Failure(testResult3, ErrorCodes2::FAILURE_3, "message alone");

    REQUIRE(testResult4.Failed());
    REQUIRE(!testResult4.Succeeded());
    REQUIRE(testResult4.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.errorCodeValue() == 1002);
    REQUIRE(testResult4.innerError());
    REQUIRE(testResult4.innerError()->Failed());
    REQUIRE(testResult4.innerError()->errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(testResult4.innerError()->message() == "message test 3 35");
    REQUIRE(testResult4.innerError()->errorCodeValue() == 1001);
    REQUIRE(!testResult4.innerError()->innerError());

    auto testResult5 = ResultWithReturnValue<ErrorCodes2, std::string>::Failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                "message {} {}", "test 4", 102);

    REQUIRE(testResult5.Failed());
    REQUIRE(!testResult5.Succeeded());
    REQUIRE(testResult5.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.errorCodeValue() == 1002);
    REQUIRE(testResult5.innerError());
    REQUIRE(testResult5.innerError()->Failed());
    REQUIRE(testResult5.innerError()->errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(testResult5.innerError()->message() == "message");
    REQUIRE(testResult5.innerError()->errorCodeValue() == 1000);
    REQUIRE(!testResult5.innerError()->innerError());
}

TEST_CASE("Result with Return Ref Test", "[basic-checks]")
{
    std::string return_value("returned value");

    auto testResult1 = ResultWithReturnRef<ErrorCodes2, std::string>(return_value);

    REQUIRE(testResult1.Succeeded());
    REQUIRE(!testResult1.Failed());
    REQUIRE(testResult1.errorCode() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.innerError());
    REQUIRE(testResult1.ReturnRef() == "returned value");
    return_value = "new value";
    REQUIRE(testResult1.ReturnRef() == "new value");

    auto testResult2 = ResultWithReturnValue<ErrorCodes2, std::string>::Failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.Failed());
    REQUIRE(!testResult2.Succeeded());
    REQUIRE(testResult2.errorCode() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.errorCodeValue() == 1000);
    REQUIRE(!testResult2.innerError());

    auto testResult3 =
        ResultWithReturnValue<ErrorCodes1, std::string>::Failure(ErrorCodes1::FAILURE_2, "message {} {}", "test 3", 35);

    REQUIRE(testResult3.Failed());
    REQUIRE(!testResult3.Succeeded());
    REQUIRE(testResult3.errorCode() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.errorCodeValue() == 1001);
    REQUIRE(!testResult3.innerError());

    auto testResult4 =
        ResultWithReturnValue<ErrorCodes2, std::string>::Failure(testResult3, ErrorCodes2::FAILURE_3, "message alone");

    REQUIRE(testResult4.Failed());
    REQUIRE(!testResult4.Succeeded());
    REQUIRE(testResult4.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.errorCodeValue() == 1002);
    REQUIRE(testResult4.innerError());
    REQUIRE(testResult4.innerError()->Failed());
    REQUIRE(testResult4.innerError()->errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(testResult4.innerError()->message() == "message test 3 35");
    REQUIRE(testResult4.innerError()->errorCodeValue() == 1001);
    REQUIRE(!testResult4.innerError()->innerError());

    auto testResult5 = ResultWithReturnValue<ErrorCodes2, std::string>::Failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                "message {} {}", "test 4", 102);

    REQUIRE(testResult5.Failed());
    REQUIRE(!testResult5.Succeeded());
    REQUIRE(testResult5.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.errorCodeValue() == 1002);
    REQUIRE(testResult5.innerError());
    REQUIRE(testResult5.innerError()->Failed());
    REQUIRE(testResult5.innerError()->errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(testResult5.innerError()->message() == "message");
    REQUIRE(testResult5.innerError()->errorCodeValue() == 1000);
    REQUIRE(!testResult5.innerError()->innerError());
}

TEST_CASE("Result with Return Unique Ptr Test", "[basic-checks]")
{
    std::unique_ptr<std::string> return_value(new std::string("returned value"));

    auto testResult1 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>(return_value);

    REQUIRE(testResult1.Succeeded());
    REQUIRE(!testResult1.Failed());
    REQUIRE(testResult1.errorCode() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.innerError());
    REQUIRE(*testResult1.ReturnPtr() == "returned value");
    return_value.reset(new std::string("new value"));
    REQUIRE(*testResult1.ReturnPtr() == "returned value");

    auto testResult2 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::Failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.Failed());
    REQUIRE(!testResult2.Succeeded());
    REQUIRE(testResult2.errorCode() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.errorCodeValue() == 1000);
    REQUIRE(!testResult2.innerError());

    auto testResult3 = ResultWithReturnUniquePtr<ErrorCodes1, std::string>::Failure(ErrorCodes1::FAILURE_2,
                                                                                    "message {} {}", "test 3", 35);

    REQUIRE(testResult3.Failed());
    REQUIRE(!testResult3.Succeeded());
    REQUIRE(testResult3.errorCode() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.errorCodeValue() == 1001);
    REQUIRE(!testResult3.innerError());

    auto testResult4 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::Failure(testResult3, ErrorCodes2::FAILURE_3,
                                                                                    "message alone");

    REQUIRE(testResult4.Failed());
    REQUIRE(!testResult4.Succeeded());
    REQUIRE(testResult4.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.errorCodeValue() == 1002);
    REQUIRE(testResult4.innerError());
    REQUIRE(testResult4.innerError()->Failed());
    REQUIRE(testResult4.innerError()->errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(testResult4.innerError()->message() == "message test 3 35");
    REQUIRE(testResult4.innerError()->errorCodeValue() == 1001);
    REQUIRE(!testResult4.innerError()->innerError());

    auto testResult5 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::Failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                    "message {} {}", "test 4", 102);

    REQUIRE(testResult5.Failed());
    REQUIRE(!testResult5.Succeeded());
    REQUIRE(testResult5.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.errorCodeValue() == 1002);
    REQUIRE(testResult5.innerError());
    REQUIRE(testResult5.innerError()->Failed());
    REQUIRE(testResult5.innerError()->errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(testResult5.innerError()->message() == "message");
    REQUIRE(testResult5.innerError()->errorCodeValue() == 1000);
    REQUIRE(!testResult5.innerError()->innerError());
}

TEST_CASE("Result with Return Shared Ptr Test", "[basic-checks]")
{
    std::shared_ptr<std::string> return_value(new std::string("returned value"));

    auto testResult1 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>(return_value);

    REQUIRE(testResult1.Succeeded());
    REQUIRE(!testResult1.Failed());
    REQUIRE(testResult1.errorCode() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.innerError());
    REQUIRE(*testResult1.ReturnPtr() == "returned value");
    *return_value = "new value";
    REQUIRE(*testResult1.ReturnPtr() == "new value");

    auto testResult2 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>::Failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.Failed());
    REQUIRE(!testResult2.Succeeded());
    REQUIRE(testResult2.errorCode() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.errorCodeValue() == 1000);
    REQUIRE(!testResult2.innerError());

    auto testResult3 = ResultWithReturnSharedPtr<ErrorCodes1, std::string>::Failure(ErrorCodes1::FAILURE_2,
                                                                                    "message {} {}", "test 3", 35);

    REQUIRE(testResult3.Failed());
    REQUIRE(!testResult3.Succeeded());
    REQUIRE(testResult3.errorCode() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.errorCodeValue() == 1001);
    REQUIRE(!testResult3.innerError());

    auto testResult4 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>::Failure(testResult3, ErrorCodes2::FAILURE_3,
                                                                                    "message alone");

    REQUIRE(testResult4.Failed());
    REQUIRE(!testResult4.Succeeded());
    REQUIRE(testResult4.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.errorCodeValue() == 1002);
    REQUIRE(testResult4.innerError());
    REQUIRE(testResult4.innerError()->Failed());
    REQUIRE(testResult4.innerError()->errorCodeType() == typeid(ErrorCodes1));
    REQUIRE(testResult4.innerError()->message() == "message test 3 35");
    REQUIRE(testResult4.innerError()->errorCodeValue() == 1001);
    REQUIRE(!testResult4.innerError()->innerError());

    auto testResult5 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>::Failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                    "message {} {}", "test 4", 102);

    REQUIRE(testResult5.Failed());
    REQUIRE(!testResult5.Succeeded());
    REQUIRE(testResult5.errorCode() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.errorCodeValue() == 1002);
    REQUIRE(testResult5.innerError());
    REQUIRE(testResult5.innerError()->Failed());
    REQUIRE(testResult5.innerError()->errorCodeType() == typeid(ErrorCodes2));
    REQUIRE(testResult5.innerError()->message() == "message");
    REQUIRE(testResult5.innerError()->errorCodeValue() == 1000);
    REQUIRE(!testResult5.innerError()->innerError());
}
