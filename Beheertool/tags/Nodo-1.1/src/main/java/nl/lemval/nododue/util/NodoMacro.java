/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.NodoCommand;

/**
 *
 * @author Michael
 */
public class NodoMacro {

    private NodoCommand event;
    private NodoCommand action;

    public NodoMacro(NodoCommand event, NodoCommand action) {
        this.event = event;
        this.action = action;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append(event);
        builder.append(" ==> ");
        builder.append(action);
        return builder.toString();
    }

    @Override
    @SuppressWarnings("EqualsWhichDoesntCheckParameterClass")
    public boolean equals(Object obj) {
        return this.toString().equals(obj.toString());
    }

    @Override
    public int hashCode() {
        return toString().hashCode();
    }

    public NodoCommand getAction() {
        return action;
    }

    public NodoCommand getEvent() {
        return event;
    }

//    public static void main(String args[]) {
//        HashMap<String, CommandInfo> col = new HashMap<String, CommandInfo>();
//        NodoMacro.loadFrom(col, "ClockSun 0,0; Alarm 6,2");
//    }
    
    /* protected */ static NodoMacro loadFrom(Map<String, CommandInfo> col, Map<String, Device> dev, String line) {
        // ClockSun 0,0; Alarm 6,2
        String[] commands = line.split("; ?");
        return loadFrom(col, dev, commands[0], commands[1]);
    }


    /* protected */ static NodoMacro loadFrom(Map<String, CommandInfo> col, Map<String, Device> dev, String cmd1, String cmd2) {
        String cmd  = "(([^ 0-9]+) ?([^, ]*),? ?([^; ]*))";
        String custom = "^(0x)?([0-9A-Fa-f]+)$";
        Pattern pattern = Pattern.compile(cmd+"|"+custom);

        NodoCommand evt = findMatch(pattern, col, dev, cmd1);
        NodoCommand act = findMatch(pattern, col, dev, cmd2);
        if ( evt != null && act != null ) {
            return new NodoMacro(evt, act);
        }
        return null;
    }

    private static NodoCommand findMatch(Pattern pattern, Map<String, CommandInfo> col, Map<String, Device> dev, String cmd) {
        Matcher matcher = pattern.matcher(cmd);
        if ( matcher.matches() ) {
            if ( matcher.group(1) == null ) {
                // Check for device
                Device device = dev.get(matcher.group(0));
                if ( device != null ) {
                    return new NodoCommand(device);
                }
                String value = matcher.group(6);
                value = "00000000".substring(value.length()) + value;
                return NodoCommand.getCustomCommand(matcher.group(5) + value.substring(0,4), value.substring(4,6), value.substring(6, 8));
            }
            Device device = dev.get(matcher.group(1));
            if ( device != null ) {
                return new NodoCommand(device);
            }
            CommandInfo ci = col.get(matcher.group(2));
            if ( ci != null ) {
                return new NodoCommand(ci, matcher.group(3), matcher.group(4));
            }
        }
        return null;
    }

    /* protected */ String getSaveFormat() {
        StringBuilder builder = new StringBuilder();
        builder.append(getEvent());
        builder.append("; ");
        builder.append(getAction());
        return builder.toString();
    }
}
