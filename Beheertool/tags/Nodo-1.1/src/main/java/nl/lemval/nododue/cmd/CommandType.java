/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;


/**
 *
 * @author Michael
 */
public class CommandType {
    public static final CommandType NOTHING = new CommandType(0);
    public static final CommandType MACRO   = new CommandType(1);
    public static final CommandType COMMAND = new CommandType(2);
    public static final CommandType SETTING = new CommandType(4);
    public static final CommandType EVENT   = new CommandType(8);
    public static final CommandType MULTI   = new CommandType(16);
    public static final CommandType SERIALONLY = new CommandType(32);
    public static final CommandType ALL = new CommandType(0xFF);

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

    public static CommandType fromString(String text) {
        if ( ROW.FlagMacro.col.equalsIgnoreCase(text) ) {
            return MACRO;
        }
        if ( ROW.FlagAction.col.equalsIgnoreCase(text) ) {
            return COMMAND;
        }
        if ( ROW.FlagSetting.col.equalsIgnoreCase(text) ) {
            return SETTING;
        }
        if ( ROW.FlagEvent.col.equalsIgnoreCase(text) ) {
            return EVENT;
        }
        if ( ROW.FlagMultiNodo.col.equalsIgnoreCase(text) ) {
            return MULTI;
        }
        if ( ROW.FlagSerialOnly.col.equalsIgnoreCase(text) ) {
            return SERIALONLY;
        }
        return NOTHING;
    }

    @Override
    public String toString() {
        StringBuilder buf = new StringBuilder(" ");
        if ( is(MACRO) ) buf.append("Macro,");
        if ( is(COMMAND) ) buf.append("Action,");
        if ( is(SETTING) ) buf.append("Setting,");
        if ( is(EVENT) ) buf.append("Event,");
        if ( is(MULTI) ) buf.append("RemoteNodo,");
        if ( is(SERIALONLY) ) buf.append("SerialOnly,");
        buf.append(Integer.toHexString(value));
        return buf.substring(0, buf.length());
    }
}
