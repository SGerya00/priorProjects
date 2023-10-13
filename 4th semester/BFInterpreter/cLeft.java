package BFinterpreter;

public class cLeft implements OpInterface {

    public void executeCommand(ExecutionContext ec) throws Exception {
        ec.decMemoryIdx();
        if (ec.getMemoryIdx() < 0) {
            throw new Exception("Attempt to access memory outside boundaries");
        }
    }

}
