/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util;

import java.util.HashMap;
import nl.lemval.nododue.util.Wire.Type;

/**
 *
 * @author Michael
 */
public class WireList {

    private HashMap<String, Wire> wires = new HashMap<String, Wire>();

    public void add(Wire w) {
        wires.put(getKey(w.getChannel(), w.getType()), w);
    }

    public Wire get(int idx, Type type) {
        Wire wire = wires.get(getKey(idx, type));
        if ( wire == null ) {
            wire = new Wire(type, idx);
            wires.put(getKey(idx, type), wire);
        }
        return wire;
    }

    private String getKey(int idx, Type type) {
        return type.name() + idx;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("WireList [");
        builder.append(wires.size());
        builder.append("]");
        return builder.toString();
    }
}
