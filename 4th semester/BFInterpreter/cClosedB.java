package BFinterpreter;

public class cClosedB implements OpInterface {

    public void executeCommand(ExecutionContext ec) throws Exception {

        if (ec.getAtMemoryIdx().value != 0) {
            int tempCounter = 0;
            do {
                if (ec.askIfLoopBegin(ec.getCommandAtIdx())) {
                    tempCounter += 1;
                } else if (ec.askIfLoopEnd(ec.getCommandAtIdx())) {
                    tempCounter -= 1;
                }
                ec.decCommandIdx();
                if (ec.getCommandIdx() < 0) {
                    throw new Exception("When executing command for LoopEnd symbol, did not find matching LoopBegin " +
                                        "symbol earlier in the programme");
                }
            } while(tempCounter != 0);
        }
    }

}
