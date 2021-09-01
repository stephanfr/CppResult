#include <catch2/catch_all.hpp>

#include "CPPResult.hpp"

//  The pragma below is to disable to false errors flagged by intellisense for
//  Catch2 REQUIRE macros.

#if __INTELLISENSE__
#pragma diag_suppress 2486
#endif

using SEFUtility::Result;
using SEFUtility::ResultBase;
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
    auto testResult1 = Result<ErrorCodes1>::success();

    REQUIRE(testResult1.succeeded());
    REQUIRE(!testResult1.failed());
    REQUIRE(testResult1.error_code() == ErrorCodes1::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.error_code_type() == typeid(ErrorCodes1));
    REQUIRE(!testResult1.inner_error());

    auto testResult1Copy(testResult1);

    REQUIRE(testResult1Copy.succeeded());
    REQUIRE(!testResult1Copy.failed());
    REQUIRE(testResult1Copy.error_code() == ErrorCodes1::SUCCESS);
    REQUIRE(testResult1Copy.message() == "Success");
    REQUIRE(testResult1Copy.error_code_type() == typeid(ErrorCodes1));
    REQUIRE(!testResult1Copy.inner_error());

    auto testResult2 = Result<ErrorCodes1>::failure(ErrorCodes1::FAILURE_1, "message");

    REQUIRE(testResult2.failed());
    REQUIRE(!testResult2.succeeded());
    REQUIRE(testResult2.error_code() == ErrorCodes1::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.error_code_value() == 1000);
    REQUIRE(!testResult2.inner_error());

    auto testResult3 = Result<ErrorCodes1>::failure(ErrorCodes1::FAILURE_2, "message {} {}", "test 3", 35);

    REQUIRE(testResult3.failed());
    REQUIRE(!testResult3.succeeded());
    REQUIRE(testResult3.error_code() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.error_code_value() == 1001);
    REQUIRE(!testResult3.inner_error());

    auto testResult4 = Result<ErrorCodes1>::failure(testResult3, ErrorCodes1::FAILURE_2, "message without var args");

    REQUIRE(testResult4.failed());
    REQUIRE(!testResult4.succeeded());
    REQUIRE(testResult4.error_code() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult4.message() == "message without var args");
    REQUIRE(testResult4.error_code_value() == 1001);
    REQUIRE(testResult4.inner_error());
    REQUIRE(testResult4.inner_error()->failed());
    REQUIRE(testResult4.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult4.inner_error()->message() == "message test 3 35");
    REQUIRE(testResult4.inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult4.inner_error()->inner_error());

    auto testResult5 =
        Result<ErrorCodes1>::failure(testResult3, ErrorCodes1::FAILURE_3, "message {} {}", "test 4", 102);

    REQUIRE(testResult5.failed());
    REQUIRE(!testResult5.succeeded());
    REQUIRE(testResult5.error_code() == ErrorCodes1::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.error_code_value() == 1002);
    REQUIRE(testResult5.inner_error());
    REQUIRE(testResult5.inner_error()->failed());
    REQUIRE(testResult5.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult5.inner_error()->message() == "message test 3 35");
    REQUIRE(testResult5.inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult5.inner_error()->inner_error());

    auto testResult6 =
        Result<ErrorCodes1>::failure(testResult5, ErrorCodes1::FAILURE_3, "message {} {} {}", "test 6", -56, true);

    REQUIRE(testResult6.failed());
    REQUIRE(!testResult6.succeeded());
    REQUIRE(testResult6.error_code() == ErrorCodes1::FAILURE_3);
    REQUIRE(testResult6.message() == "message test 6 -56 true");
    REQUIRE(testResult6.error_code_value() == 1002);
    REQUIRE(testResult6.inner_error());
    REQUIRE(testResult6.inner_error()->failed());
    REQUIRE(testResult6.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult6.inner_error()->message() == "message test 4 102");
    REQUIRE(testResult6.inner_error()->error_code_value() == 1002);
    REQUIRE(testResult6.inner_error()->inner_error());
    REQUIRE(testResult6.inner_error()->inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult6.inner_error()->inner_error()->message() == "message test 3 35");
    REQUIRE(testResult6.inner_error()->inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult6.inner_error()->inner_error()->inner_error());

    testResult1 = testResult6;

    REQUIRE(testResult1.failed());
    REQUIRE(!testResult1.succeeded());
    REQUIRE(testResult1.error_code() == ErrorCodes1::FAILURE_3);
    REQUIRE(testResult1.message() == "message test 6 -56 true");
    REQUIRE(testResult1.error_code_value() == 1002);
    REQUIRE(testResult1.inner_error());
    REQUIRE(testResult1.inner_error()->failed());
    REQUIRE(testResult1.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult1.inner_error()->message() == "message test 4 102");
    REQUIRE(testResult1.inner_error()->error_code_value() == 1002);
    REQUIRE(testResult1.inner_error()->inner_error());
    REQUIRE(testResult1.inner_error()->inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult1.inner_error()->inner_error()->message() == "message test 3 35");
    REQUIRE(testResult1.inner_error()->inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult1.inner_error()->inner_error()->inner_error());
}

TEST_CASE("Result with Return Value Test", "[basic-checks]")
{
    std::string return_value("returned value");

    auto testResult1 = ResultWithReturnValue<ErrorCodes2, std::string>::success(return_value);

    REQUIRE(testResult1.succeeded());
    REQUIRE(!testResult1.failed());
    REQUIRE(testResult1.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.inner_error());
    REQUIRE(testResult1.return_value() == "returned value");
    return_value = "new value";
    REQUIRE(testResult1.return_value() == "returned value");

    auto testResult1Copy(testResult1);

    REQUIRE(testResult1Copy.succeeded());
    REQUIRE(!testResult1Copy.failed());
    REQUIRE(testResult1Copy.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1Copy.message() == "Success");
    REQUIRE(testResult1Copy.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1Copy.inner_error());
    REQUIRE(testResult1Copy.return_value() == "returned value");

    auto testResult2 = ResultWithReturnValue<ErrorCodes2, std::string>::failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.failed());
    REQUIRE(!testResult2.succeeded());
    REQUIRE(testResult2.error_code() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.error_code_value() == 1000);
    REQUIRE(!testResult2.inner_error());

    auto testResult3 =
        ResultWithReturnValue<ErrorCodes1, std::string>::failure(ErrorCodes1::FAILURE_2, "message {} {}", "test 3", 35);

    REQUIRE(testResult3.failed());
    REQUIRE(!testResult3.succeeded());
    REQUIRE(testResult3.error_code() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.error_code_value() == 1001);
    REQUIRE(!testResult3.inner_error());

    auto testResult4 =
        ResultWithReturnValue<ErrorCodes2, std::string>::failure(testResult3, ErrorCodes2::FAILURE_3, "message alone");

    REQUIRE(testResult4.failed());
    REQUIRE(!testResult4.succeeded());
    REQUIRE(testResult4.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.error_code_value() == 1002);
    REQUIRE(testResult4.inner_error());
    REQUIRE(testResult4.inner_error()->failed());
    REQUIRE(testResult4.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult4.inner_error()->message() == "message test 3 35");
    REQUIRE(testResult4.inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult4.inner_error()->inner_error());

    auto testResult5 = ResultWithReturnValue<ErrorCodes2, std::string>::failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                "message {} {}", "test 4", 102);

    REQUIRE(testResult5.failed());
    REQUIRE(!testResult5.succeeded());
    REQUIRE(testResult5.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.error_code_value() == 1002);
    REQUIRE(testResult5.inner_error());
    REQUIRE(testResult5.inner_error()->failed());
    REQUIRE(testResult5.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult5.inner_error()->message() == "message");
    REQUIRE(testResult5.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult5.inner_error()->inner_error());

    testResult1 = testResult5;

    REQUIRE(testResult1.failed());
    REQUIRE(!testResult1.succeeded());
    REQUIRE(testResult1.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult1.message() == "message test 4 102");
    REQUIRE(testResult1.error_code_value() == 1002);
    REQUIRE(testResult1.inner_error());
    REQUIRE(testResult1.inner_error()->failed());
    REQUIRE(testResult1.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult1.inner_error()->message() == "message");
    REQUIRE(testResult1.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult1.inner_error()->inner_error());
}

TEST_CASE("Result with Return Ref Test", "[basic-checks]")
{
    std::string return_value("returned value");

    auto testResult1 = ResultWithReturnRef<ErrorCodes2, std::string>(return_value);

    REQUIRE(testResult1.succeeded());
    REQUIRE(!testResult1.failed());
    REQUIRE(testResult1.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.inner_error());
    REQUIRE(testResult1.return_ref() == "returned value");
    return_value = "new value";
    REQUIRE(testResult1.return_ref() == "new value");

    auto testResult1Copy(testResult1);

    REQUIRE(testResult1Copy.succeeded());
    REQUIRE(!testResult1Copy.failed());
    REQUIRE(testResult1Copy.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1Copy.message() == "Success");
    REQUIRE(testResult1Copy.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1Copy.inner_error());
    REQUIRE(testResult1Copy.return_ref() == "new value");

    auto testResult2 = ResultWithReturnRef<ErrorCodes2, std::string>::failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.failed());
    REQUIRE(!testResult2.succeeded());
    REQUIRE(testResult2.error_code() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.error_code_value() == 1000);
    REQUIRE(!testResult2.inner_error());

    auto testResult3 =
        ResultWithReturnRef<ErrorCodes1, std::string>::failure(ErrorCodes1::FAILURE_2, "message {} {}", "test 3", 35);

    REQUIRE(testResult3.failed());
    REQUIRE(!testResult3.succeeded());
    REQUIRE(testResult3.error_code() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.error_code_value() == 1001);
    REQUIRE(!testResult3.inner_error());

    auto testResult4 =
        ResultWithReturnRef<ErrorCodes2, std::string>::failure(testResult3, ErrorCodes2::FAILURE_3, "message alone");

    REQUIRE(testResult4.failed());
    REQUIRE(!testResult4.succeeded());
    REQUIRE(testResult4.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.error_code_value() == 1002);
    REQUIRE(testResult4.inner_error());
    REQUIRE(testResult4.inner_error()->failed());
    REQUIRE(testResult4.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult4.inner_error()->message() == "message test 3 35");
    REQUIRE(testResult4.inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult4.inner_error()->inner_error());

    auto testResult5 = ResultWithReturnRef<ErrorCodes2, std::string>::failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                "message {} {}", "test 4", 102);

    REQUIRE(testResult5.failed());
    REQUIRE(!testResult5.succeeded());
    REQUIRE(testResult5.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.error_code_value() == 1002);
    REQUIRE(testResult5.inner_error());
    REQUIRE(testResult5.inner_error()->failed());
    REQUIRE(testResult5.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult5.inner_error()->message() == "message");
    REQUIRE(testResult5.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult5.inner_error()->inner_error());

    testResult1 = testResult5;

    REQUIRE(testResult1.failed());
    REQUIRE(!testResult1.succeeded());
    REQUIRE(testResult1.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult1.message() == "message test 4 102");
    REQUIRE(testResult1.error_code_value() == 1002);
    REQUIRE(testResult1.inner_error());
    REQUIRE(testResult1.inner_error()->failed());
    REQUIRE(testResult1.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult1.inner_error()->message() == "message");
    REQUIRE(testResult1.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult1.inner_error()->inner_error());
}

TEST_CASE("Result with Return Unique Ptr Test", "[basic-checks]")
{
    std::unique_ptr<std::string> return_value(new std::string("returned value"));

    auto testResult1 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::success( std::move( return_value));

    REQUIRE(testResult1.succeeded());
    REQUIRE(!testResult1.failed());
    REQUIRE(testResult1.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.inner_error());
    REQUIRE(*testResult1.return_ptr() == "returned value");
    return_value.reset(new std::string("new value"));
    REQUIRE(*testResult1.return_ptr() == "returned value");

    auto testResult1Copy( testResult1 );

    REQUIRE(testResult1Copy.succeeded());
    REQUIRE(!testResult1Copy.failed());
    REQUIRE(testResult1Copy.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1Copy.message() == "Success");
    REQUIRE(testResult1Copy.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1Copy.inner_error());
    REQUIRE(*testResult1Copy.return_ptr() == "returned value");

    auto testResult2 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.failed());
    REQUIRE(!testResult2.succeeded());
    REQUIRE(testResult2.error_code() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.error_code_value() == 1000);
    REQUIRE(!testResult2.inner_error());

    auto testResult3 = ResultWithReturnUniquePtr<ErrorCodes1, std::string>::failure(ErrorCodes1::FAILURE_2,
                                                                                    "message {} {}", "test 3", 35);

    REQUIRE(testResult3.failed());
    REQUIRE(!testResult3.succeeded());
    REQUIRE(testResult3.error_code() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.error_code_value() == 1001);
    REQUIRE(!testResult3.inner_error());

    auto testResult4 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::failure(testResult3, ErrorCodes2::FAILURE_3,
                                                                                    "message alone");

    REQUIRE(testResult4.failed());
    REQUIRE(!testResult4.succeeded());
    REQUIRE(testResult4.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.error_code_value() == 1002);
    REQUIRE(testResult4.inner_error());
    REQUIRE(testResult4.inner_error()->failed());
    REQUIRE(testResult4.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult4.inner_error()->message() == "message test 3 35");
    REQUIRE(testResult4.inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult4.inner_error()->inner_error());

    auto testResult5 = ResultWithReturnUniquePtr<ErrorCodes2, std::string>::failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                    "message {} {}", "test 4", 102);

    REQUIRE(testResult5.failed());
    REQUIRE(!testResult5.succeeded());
    REQUIRE(testResult5.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.error_code_value() == 1002);
    REQUIRE(testResult5.inner_error());
    REQUIRE(testResult5.inner_error()->failed());
    REQUIRE(testResult5.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult5.inner_error()->message() == "message");
    REQUIRE(testResult5.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult5.inner_error()->inner_error());
}

TEST_CASE("Result with Return Shared Ptr Test", "[basic-checks]")
{
    std::shared_ptr<std::string> return_value(new std::string("returned value"));

    auto testResult1 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>(return_value);

    REQUIRE(testResult1.succeeded());
    REQUIRE(!testResult1.failed());
    REQUIRE(testResult1.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1.message() == "Success");
    REQUIRE(testResult1.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1.inner_error());
    REQUIRE(*testResult1.return_ptr() == "returned value");
    *return_value = "new value";
    REQUIRE(*testResult1.return_ptr() == "new value");

    auto testResult1Copy( testResult1 );

    REQUIRE(testResult1Copy.succeeded());
    REQUIRE(!testResult1Copy.failed());
    REQUIRE(testResult1Copy.error_code() == ErrorCodes2::SUCCESS);
    REQUIRE(testResult1Copy.message() == "Success");
    REQUIRE(testResult1Copy.error_code_type() == typeid(ErrorCodes2));
    REQUIRE(!testResult1Copy.inner_error());
    REQUIRE(*testResult1Copy.return_ptr() == "new value");
    *return_value = "new value 2";
    REQUIRE(*testResult1Copy.return_ptr() == "new value 2");

    auto testResult2 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>::failure(ErrorCodes2::FAILURE_1, "message");

    REQUIRE(testResult2.failed());
    REQUIRE(!testResult2.succeeded());
    REQUIRE(testResult2.error_code() == ErrorCodes2::FAILURE_1);
    REQUIRE(testResult2.message() == "message");
    REQUIRE(testResult2.error_code_value() == 1000);
    REQUIRE(!testResult2.inner_error());

    auto testResult3 = ResultWithReturnSharedPtr<ErrorCodes1, std::string>::failure(ErrorCodes1::FAILURE_2,
                                                                                    "message {} {}", "test 3", 35);

    REQUIRE(testResult3.failed());
    REQUIRE(!testResult3.succeeded());
    REQUIRE(testResult3.error_code() == ErrorCodes1::FAILURE_2);
    REQUIRE(testResult3.message() == "message test 3 35");
    REQUIRE(testResult3.error_code_value() == 1001);
    REQUIRE(!testResult3.inner_error());

    auto testResult4 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>::failure(testResult3, ErrorCodes2::FAILURE_3,
                                                                                    "message alone");

    REQUIRE(testResult4.failed());
    REQUIRE(!testResult4.succeeded());
    REQUIRE(testResult4.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult4.message() == "message alone");
    REQUIRE(testResult4.error_code_value() == 1002);
    REQUIRE(testResult4.inner_error());
    REQUIRE(testResult4.inner_error()->failed());
    REQUIRE(testResult4.inner_error()->error_code_type() == typeid(ErrorCodes1));
    REQUIRE(testResult4.inner_error()->message() == "message test 3 35");
    REQUIRE(testResult4.inner_error()->error_code_value() == 1001);
    REQUIRE(!testResult4.inner_error()->inner_error());

    auto testResult5 = ResultWithReturnSharedPtr<ErrorCodes2, std::string>::failure(testResult2, ErrorCodes2::FAILURE_3,
                                                                                    "message {} {}", "test 4", 102);

    REQUIRE(testResult5.failed());
    REQUIRE(!testResult5.succeeded());
    REQUIRE(testResult5.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult5.message() == "message test 4 102");
    REQUIRE(testResult5.error_code_value() == 1002);
    REQUIRE(testResult5.inner_error());
    REQUIRE(testResult5.inner_error()->failed());
    REQUIRE(testResult5.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult5.inner_error()->message() == "message");
    REQUIRE(testResult5.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult5.inner_error()->inner_error());

    testResult1 = testResult5;

    REQUIRE(testResult1.failed());
    REQUIRE(!testResult1.succeeded());
    REQUIRE(testResult1.error_code() == ErrorCodes2::FAILURE_3);
    REQUIRE(testResult1.message() == "message test 4 102");
    REQUIRE(testResult1.error_code_value() == 1002);
    REQUIRE(testResult1.inner_error());
    REQUIRE(testResult1.inner_error()->failed());
    REQUIRE(testResult1.inner_error()->error_code_type() == typeid(ErrorCodes2));
    REQUIRE(testResult1.inner_error()->message() == "message");
    REQUIRE(testResult1.inner_error()->error_code_value() == 1000);
    REQUIRE(!testResult1.inner_error()->inner_error());
}
