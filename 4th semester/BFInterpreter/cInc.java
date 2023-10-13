package BFinterpreter;

public class cInc implements OpInterface {

    public void executeCommand(ExecutionContext ec) {
        ec.getAtMemoryIdx().value += 1;
    }

}
