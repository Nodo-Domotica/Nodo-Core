/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util;

import java.util.ArrayList;

/**
 *
 * @author Michael
 */
public class Wire {

    public enum Type {
        Input, Output;
    };

    private static int maxLength = 0;

    private Type type;
    private int channel;
    private InputRange signalType;

    private ArrayList<WireData> wireData = new ArrayList<WireData>();

    public Wire(Type t, int c) {
        this.type = t;
        this.channel = c;
    }

    public int getChannel() {
        return channel;
    }

    public Type getType() {
        return type;
    }

    public InputRange getSignalType() {
        return signalType;
    }

    public void setSignalType(InputRange signalType) {
        this.signalType = signalType;
    }

    public void addData(WireData data) {
        wireData.add(data);
//        System.out.println(this + ", added " + data + " (" + maxLength + ")");
        while ( wireData.size() > maxLength ) {
            wireData.remove(0);
        }
    }

    public static void setMaxLength(int max) {
        if ( maxLength <= 0 )
            maxLength = max;
    }

    public ArrayList<WireData> getWireData() {
        return wireData;
    }

    public String getLastValue() {
        int idx = wireData.size()-1;
        if ( idx >= 0 ) {
            int val = wireData.get(idx).getValue();
            return String.valueOf(val);
        }
        return "?";
    }

    public String getActive() {
        int idx = wireData.size()-1;
        if ( idx >= 0 ) {
            WireData wd = wireData.get(idx);
            return (wd.isState() ? "J":"N");
        }
        return "?";
    }

    public WireData getLastData() {
        int idx = wireData.size()-1;
        if ( idx >= 0 )
            return wireData.get(idx);
        return null;
    }

    @Override
    public String toString() {
        return getType() + " wire " + getChannel() + " (" + wireData.size() + ", " + getLastValue() + ")";
    }
}
