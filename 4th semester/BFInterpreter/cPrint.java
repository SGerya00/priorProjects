package BFinterpreter;

public class cPrint implements OpInterface {

    public void executeCommand(ExecutionContext ec) {
        ec.putInPrintStream(ec.getAtMemoryIdx().value);
    }

}
