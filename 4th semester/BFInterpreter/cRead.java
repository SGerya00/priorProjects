package BFinterpreter;

import java.io.IOException;

public class cRead implements OpInterface {

    public void executeCommand(ExecutionContext ec) throws IOException {
        int temp = ec.getFromInputStream();
        ec.getAtMemoryIdx().value = (char)temp;
    }

}
