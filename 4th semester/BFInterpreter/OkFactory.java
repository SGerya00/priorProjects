package BFinterpreter;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class OkFactory {

    void fillMap(BufferedReader reader) throws Exception {
        try {
            int counter = 0;
            String line = null;
            while ((line = reader.readLine()) != null) {
                if (!checkFormat(line)) {
                    throw new Exception("Line format error in line " + (counter + 1) + " in configuration file");
                }
                char c = line.charAt(0);
                StringBuilder fullName = new StringBuilder(line);
                fullName.delete(0, 2); //char and a subsequent '='
                if (line.charAt(line.length() - 1) == '+') {
                    loopBeginChar = c;
                    fullName.delete(fullName.length() - 2, fullName.length());
                    charToFullNameMap.put(c, String.valueOf(fullName));
                } else if (line.charAt(line.length() - 1) == '-') {
                    loopEndChar = c;
                    fullName.delete(fullName.length() - 2, fullName.length());
                    charToFullNameMap.put(c, String.valueOf(fullName));
                } else {
                    charToFullNameMap.put(c, String.valueOf(fullName));
                }
                counter += 1;
            }
        } catch (IOException x) {
            throw new Exception("Trouble reading from given configuration file");
        }
    }

    OpInterface getFromFactory(char c) throws Exception {
        OpInterface temp = cache.findOpByChar(c);
        if (temp == null) {
            String fullName = charToFullNameMap.get(c);
            if (fullName == null) {
                throw new Exception("Did not find class responsible for executing '" +
                                    c + "' command in configuration file");
            }
            Class<?> neededClass = null;
            try {
                neededClass = Class.forName(fullName);
            } catch (ClassNotFoundException e) {
                throw new Exception("Class \"" + fullName + "\" not found");
            }
            OpInterface neededObject = (OpInterface) neededClass.getDeclaredConstructor().newInstance();
            cache.addToCache(neededObject, c);
            return neededObject;
        } else {
            return temp;
        }
    }

    protected boolean checkFormat(String line) {
        return (line.charAt(1) == '=');
    }

    boolean askIfLoopEnd(char c) {
        return (c == loopEndChar);
    }
    boolean askIfLoopBegin(char c) {
        return (c == loopBeginChar);
    }

    private final OpCache cache = new OpCache();
    private final Map<Character, String> charToFullNameMap = new HashMap<>();

    private char loopBeginChar = 0;
    private char loopEndChar = 0;

}
