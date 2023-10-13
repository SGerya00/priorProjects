package BFinterpreter;

public class ExecutionContext {

    ExecutionContext(String programme, OkFactory f) {
        memory = new Data[MAX_MEMORY];
        for (int i = 0; i < MAX_MEMORY; i++) {
            memory[i] = new Data();
        }
        memoryIdx = 0;

        commandSequence = programme;
        commandIdxMax = commandSequence.length();
        commandIdx = 0;

        printStream = System.out;
        inputStream = System.in;

        factory = f;
    }

    boolean askIfLoopEnd(char c) {
        return factory.askIfLoopEnd(c);
    }

    boolean askIfLoopBegin(char c) {
        return factory.askIfLoopBegin(c);
    }

    private final int MAX_MEMORY = 30000;
    int getMAX_MEMORY() {
        return MAX_MEMORY;
    }

    private final Data[] memory;
    private int memoryIdx;
    int getMemoryIdx() {
        return memoryIdx;
    }
    void incMemoryIdx() {
        memoryIdx += 1;
    }
    void decMemoryIdx() {
        memoryIdx -= 1;
    }
    Data getAtMemoryIdx() {
        return memory[memoryIdx];
    }

    private final String commandSequence;
    private final int commandIdxMax;
    int getCommandIdxMax() {
        return commandIdxMax;
    }
    private int commandIdx;
    int getCommandIdx() {
        return commandIdx;
    }
    void incCommandIdx() {
        commandIdx += 1;
    }
    void decCommandIdx() {
        commandIdx -= 1;
    }
    char getCommandAtIdx() {
        return commandSequence.charAt(commandIdx);
    }

    private final java.io.PrintStream printStream;
    void putInPrintStream(char c) {
        printStream.print(c);
    }
    private final java.io.InputStream inputStream;
    int getFromInputStream() {
        int temp = 0;
        try {
            temp = inputStream.read();
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return -1;
        }
        return temp;
    }

    private final OkFactory factory;
}
