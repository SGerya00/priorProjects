package BFinterpreter;

public class MyPair {

    MyPair(char newc, OpInterface newcommand) {
        c = newc;
        command = newcommand;
    }

    public char first() {
        return c;
    }

    public OpInterface second() {
        return command;
    }

    char c = 0;
    OpInterface command = null;

}
