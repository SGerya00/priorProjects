package BFinterpreter;

public class cOpenB implements OpInterface {

    public void executeCommand(ExecutionContext ec) throws Exception {

        if (ec.getAtMemoryIdx().value == 0) {
            int tempCounter = 0;
            while(true) {
                if (ec.askIfLoopBegin(ec.getCommandAtIdx())) {
                    tempCounter += 1;
                } else if (ec.askIfLoopEnd(ec.getCommandAtIdx())) {
                    tempCounter -= 1;
                    if (tempCounter == 0) {
                        return;
                    }
                }
                ec.incCommandIdx();
                if (ec.getCommandIdx() >= ec.getCommandIdxMax()) {
                    throw new Exception("When executing command for LoopBegin symbol, did not find matching LoopEnd " +
                                        "symbol further in the programme");
                }
            }
        }
    }

}
