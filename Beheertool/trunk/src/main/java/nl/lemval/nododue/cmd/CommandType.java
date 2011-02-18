/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import java.util.HashMap;


/**
 *
 * @author Michael
 */
public class CommandType {
    public static final CommandType NOTHING = new CommandType(0);
    public static final CommandType COMMAND = new CommandType(2);
    public static final CommandType SETTING = new CommandType(4);
    public static final CommandType EVENT   = new CommandType(8);
    public static final CommandType MULTI   = new CommandType(16);
    public static final CommandType SERIALONLY = new CommandType(32);
    public static final CommandType DEVICE  = new CommandType(64);
    public static final CommandType ALL = new CommandType(0xFF);

    private static final HashMap<String, CommandType> mapping;
    static {
        mapping = new HashMap<String, CommandType>();
        mapping.put(ROW.FlagAction.name(), COMMAND);
        mapping.put(ROW.FlagSetting.name(), SETTING);
        mapping.put(ROW.FlagEvent.name(), EVENT);
        mapping.put(ROW.FlagMultiNodo.name(), MULTI);
        mapping.put(ROW.FlagSerialOnly.name(), SERIALONLY);
        mapping.put(ROW.FlagDevice.name(), DEVICE);
    };
    
    static CommandType fromString(String validator) {
        return mapping.get(validator);
    }

    private int value;

    private CommandType(int value) {
        this.value = value;
    }

    public CommandType add(CommandType type) {
        return new CommandType(value | type.value);
    }

    public CommandType remove(CommandType type) {
        return new CommandType(value & ~type.value);
    }

    public boolean is(CommandType type) {
//        System.out.println("Type  = " + Integer.toBinaryString(type.value));
//        System.out.println("Cmd   = " + Integer.toBinaryString(value));
//        System.out.println("Result= " + Integer.toBinaryString(value&type.value));
        return (value & type.value) > 0;
    }

    @Override
    public boolean equals(Object obj) {
        throw new NullPointerException();
//        return super.equals(obj);
    }
    
    @Override
    public String toString() {
        StringBuilder buf = new StringBuilder(" ");
        if ( is(DEVICE) ) buf.append("Device,");
        if ( is(COMMAND) ) buf.append("Action,");
        if ( is(SETTING) ) buf.append("Setting,");
        if ( is(EVENT) ) buf.append("Event,");
        if ( is(MULTI) ) buf.append("RemoteNodo,");
        if ( is(SERIALONLY) ) buf.append("SerialOnly,");
        buf.append(Integer.toBinaryString(value));
        return buf.substring(0, buf.length());
    }
}
