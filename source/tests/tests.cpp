#include "tests/testbase.hpp"
#include "tests/tests.hpp"
#include "tests/files/BoxInfoTest.hpp"
#include "tests/files/EventLogTest.hpp"
#include "tests/files/MBoxData001.hpp"
#include "tests/files/MBoxData010.hpp"
#include "tests/files/MBoxData050.hpp"
#include "tests/files/MBoxInfoTest.hpp"
#include "tests/files/MBoxListTest.hpp"
#include "tests/files/OBIndexTest.hpp"
#include "tests/functions/Delete.hpp"
#include "tests/functions/GetCecdState.hpp"
#include "tests/functions/GetCecInfoBuffer.hpp"
#include "tests/functions/GetCecInfoEventHandle.hpp"
#include "tests/functions/GetChangeStateEventHandle.hpp"
#include "tests/functions/GetEventLog.hpp"
#include "tests/functions/GetEventLogStart.hpp"
#include "tests/functions/Open.hpp"
#include "tests/functions/OpenAndRead.hpp"
#include "tests/functions/OpenAndWrite.hpp"
#include "tests/functions/Read.hpp"
#include "tests/functions/ReadData.hpp"
#include "tests/functions/ReadMessage.hpp"
#include "tests/functions/ReadMessageWithHMAC.hpp"
#include "tests/functions/SetData.hpp"
#include "tests/functions/Start.hpp"
#include "tests/functions/Stop.hpp"
#include "tests/functions/Write.hpp"
#include "tests/functions/WriteMessage.hpp"
#include "tests/functions/WriteMessageWithHMAC.hpp"

#include <sys/stat.h>

namespace Tests {

void RunAllTests() {
    printf("Running All Tests\n");

    // Make sure test output dir "/3ds/CECTool/tests/" exists
    mkdir(rootTestsPath.c_str(), 777);

    RunAllFileTests();
    RunAllFunctionTests();

    printf("All Tests complete\n");
}

void RunAllFileTests() {
    printf("Running All File Tests\n");
    const std::string fileTestsPath = rootTestsPath + "files/";
    mkdir(fileTestsPath.c_str(), 777);

    std::string logPath = fileTestsPath + "BoxInfo.txt";
    TestBase* Test = new BoxInfoTest(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "EventLog.txt";
    Test = new EventLogTest(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "MBoxData001.txt";
    Test = new MBoxData001(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "MBoxData010.txt";
    Test = new MBoxData010(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "MBoxData050.txt";
    Test = new MBoxData050(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "MBoxInfo.txt";
    Test = new MBoxInfoTest(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "MBoxList.txt";
    Test = new MBoxListTest(logPath);
    Test->RunTest();
    delete Test;

    logPath = fileTestsPath + "OBIndex.txt";
    Test = new OBIndexTest(logPath);
    Test->RunTest();
    delete Test;

    printf("All File Tests Complete\n");
}

void RunAllFunctionTests() {
    printf("Running All Function Tests\n");
    const std::string functionTestsPath = rootTestsPath + "functions/";
    mkdir(functionTestsPath.c_str(), 777);

    std::string logPath = functionTestsPath + "Delete.txt";
    TestBase* Test = new Delete(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "GetCecdState.txt";
    Test = new GetCecdState(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "GetCecInfoBuffer.txt";
    Test = new GetCecInfoBuffer(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "GetCecInfoEventHandle.txt";
    Test = new GetCecInfoEventHandle(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "GetChangeStateEventHandle.txt";
    Test = new GetChangeStateEventHandle(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "GetEventLog.txt";
    Test = new GetEventLog(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "GetEventLogStart.txt";
    Test = new GetEventLogStart(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "Open.txt";
    Test = new Open(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "OpenAndRead.txt";
    Test = new OpenAndRead(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "OpenAndWrite.txt";
    Test = new OpenAndWrite(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "Read.txt";
    Test = new Read(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "ReadData.txt";
    Test = new ReadData(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "ReadMessage.txt";
    Test = new ReadMessage(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "ReadMessageWithHMAC.txt";
    Test = new ReadMessageWithHMAC(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "SetData.txt";
    Test = new SetData(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "Start.txt";
    Test = new Start(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "Stop.txt";
    Test = new Stop(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "Write.txt";
    Test = new Write(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "WriteMessage.txt";
    Test = new WriteMessage(logPath);
    Test->RunTest();
    delete Test;

    logPath = functionTestsPath + "WriteMessageWithHMAC.txt";
    Test = new WriteMessageWithHMAC(logPath);
    Test->RunTest();
    delete Test;

    printf("All Function Tests Complete\n");
}

} // namespace Tests
