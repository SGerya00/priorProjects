package BFinterpreter;

import java.util.LinkedList;

public class OpCache {
    OpCache() {
        cache = new LinkedList<>();
    }

    void addToCache(OpInterface o, char c) {
        boolean foundFlag = false;
        for (MyPair myPair : cache) {
            if (myPair.second() == o) {
                foundFlag = true;
                break;
            }
        }
        if (!foundFlag) {
            cache.addFirst(new MyPair(c, o));
        }
    }

    OpInterface findOpByChar(char c) {
        for (int i = 0; i < cache.size(); i++) {
            if (cache.get(i).first() == c) {
                return getAndForward(i);
            }
        }
        return null;
    }

    private OpInterface getAndForward(int idx) {
        MyPair temp = cache.remove(idx);
        cache.addFirst(temp);
        return cache.getFirst().second();
    }

    private final LinkedList<MyPair> cache;
}
