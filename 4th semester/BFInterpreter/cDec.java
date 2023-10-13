package BFinterpreter;

public class cDec implements OpInterface {

    public void executeCommand(ExecutionContext ec) {
        ec.getAtMemoryIdx().value -= 1;
    }

}
