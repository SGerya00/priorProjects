package BFinterpreter;

public class cRight implements OpInterface {

    public void executeCommand(ExecutionContext ec) throws Exception {
        ec.incMemoryIdx();
        if (ec.getMemoryIdx() >= ec.getMAX_MEMORY()) {
            throw new Exception("Attempt to access memory outside boundaries");
        }
    }

}
