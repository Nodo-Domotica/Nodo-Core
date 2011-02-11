/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import java.security.AccessControlException;
import java.util.HashSet;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.util.Device;

/**
 *
 * @author Michael
 */
public class NodoCommand {

    private String name;
    private String data1;
    private String data2;
    private boolean local = true;
    private boolean custom = false;
    private HashSet<Integer> units;
    private boolean useLocalUnit = false;

    private static final String NEWLINE = System.getProperty("line.separator");
    
    public NodoCommand(CommandInfo action, String d1, String d2) {
        this(action.toString(), d1, d2);
        this.local = action.getType().is(CommandType.SERIALONLY);
    }

    private NodoCommand(String action, String d1, String d2) {
        this.name = action;
        this.data1 = d1; // toHexValue(d1);
        this.data2 = d2; // toHexValue(d2);
        this.units = new HashSet<Integer>();

	CommandInfo info = CommandLoader.get(action);
	if ( info != null ) {
	    this.local =  info.getType().is(CommandType.SERIALONLY);
	}
    }

    public NodoCommand(Device device) {
        this.name = device.getSignal();
    }

    public boolean matches(Device device) {
        return name.equals(device.getSignal()) || toString().equals(device.getSignal());
    }

    public void setData(String one, String two) {
        if ( one != null ) {
            if ( data1 != null )
                throw new AccessControlException("Already set");
            data1 = one;
        }
        if ( two != null ) {
            if ( data2 != null )
                throw new AccessControlException("Already set");
            data2 = two;
        }
    }

    public static NodoCommand getStatusEventCommand(CommandInfo setting) {
        CommandInfo stevt = CommandLoader.get(CommandInfo.Name.EventStatus);
        return new NodoCommand(stevt, setting.getName(), null);
    }

    public static NodoCommand getStatusCommand(CommandInfo setting) {
        CommandInfo status = CommandLoader.get(CommandInfo.Name.Status);
        return new NodoCommand(status, setting.getName(), null);
    }

    public static NodoCommand getStatusCommand(CommandInfo setting, int item) {
        CommandInfo status = CommandLoader.get(CommandInfo.Name.Status);
        return new NodoCommand(status, setting.getName(), String.valueOf(item));
    }

    public static NodoCommand getRemoteStatusCommand(CommandInfo setting, int item) {
        CommandInfo status = CommandLoader.get(CommandInfo.Name.SendStatus);
        return new NodoCommand(status, setting.getName(), String.valueOf(item));
    }

    public static NodoCommand getCustomCommand(String cmd, String v1, String v2) {
        NodoCommand nc = new NodoCommand(cmd, v1, v2);
        nc.custom = true;
        // TODO: Ouch. I do not know if the created comment is local or not.
        // Let the Nodo decide than...
        nc.local = false;
        System.out.println("Created CUSTOM with: " + nc.getName() + ":" + nc.getData1() + ":" + nc.getData2());
        return nc;
    }

    public void addUnit(int i) {
        if ( i >= 0 && i < 16 ) {
            units.add(i);
        }
    }

    private void addCurrentUnit() {
	this.useLocalUnit = true;
    }

    private int[] getUnits() {
        Integer[] ints = units.toArray(new Integer[0]);
        int[] result = new int[ints.length];
        for (int i = 0; i < ints.length; i++) {
            result[i] = ints[i].intValue();
        }
        return result;
    }

    public String getRawCommand() {
        StringBuilder builder = new StringBuilder();
	if ( local ) {
	    addCommand(builder);
	    return builder.toString();
	}

        int[] list = getUnits();
        if ( list.length > 0 ) {
	    if ( useLocalUnit ) {
		addCommand(builder);
		builder.append(NEWLINE);
            }
            for (int i = 0; i < list.length; i++) {
                int unit = list[i];
		builder.append("Divert");
		builder.append(" ");
		builder.append(unit);
		builder.append("; ");
                addCommand(builder);

                if ( i < (list.length-1) ) {
                    builder.append(NEWLINE);
                }
            }
        } else {
            addCommand(builder);
        }
        return builder.toString();
    }

    private void addCommand(StringBuilder builder) {
        builder.append(toString());
        builder.append(';');
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        if ( custom ) {
            if ( name == null ) {
                builder.append("0x0000");
            } else {
                if ( name.startsWith("0x") ) {
                    builder.append("0000".substring(name.length()-2));
                } else {
                    builder.append("0x");
                    builder.append("0000".substring(name.length()));
                }
                builder.append(name);
            }
            if ( data1 == null ) {
                builder.append("00");
            } else {
                builder.append("00".substring(data1.length()));
                builder.append(data1);
            }
            if ( data2 == null ) {
                builder.append("00");
            } else {
                builder.append("00".substring(data2.length()));
                builder.append(data2);
            }
        } else {
            builder.append(name);
            if ( data1 != null && data1.trim().length() > 0 ) {
                builder.append(" ");
                builder.append(data1);
                if ( data2 != null && data2.trim().length() > 0 ) {
                    builder.append(",");
                    builder.append(data2);
                }
            }
        }
        return builder.toString();
    }

    public String getName() {
        return name;
    }

    public String getData1() {
        return data1;
    }

    public String getData2() {
        return data2;
    }

    /**
     * Normally, you have to specify to which unit the command is sent.
     * By default, this is the local unit.
     * This method reads the options and updates the command to allow
     * for distribution of the command.
     * Note that local commands cannot be distributed
     */
    public void makeDistributed() {
        Options options = Options.getInstance();
        if ( options.isUseRemoteUnits() ) {
            String[] list = options.getRemoteUnits();
            if (list.length > 0 && options.isUseLocalUnit()) {
                addCurrentUnit();
            }
            for (int i = 0; i < list.length; i++) {
                addUnit(Integer.parseInt(list[i], 16));
            }
        }
    }
}