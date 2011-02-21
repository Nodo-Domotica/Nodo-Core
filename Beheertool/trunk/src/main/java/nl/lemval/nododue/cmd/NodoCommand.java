/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.cmd;

import java.security.AccessControlException;
import java.util.HashSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.util.Device;

/**
 *
 * @author Michael
 */
public class NodoCommand {

//    private static final String NEWLINE = System.getProperty("line.separator");
    private static final Pattern elementPattern = Pattern.compile("\\(([A-Za-z]+) ([^,; \\)]*),? ?([^,; \\)]*)\\)");
    private static final Pattern hexPattern = Pattern.compile("\\(0x([A-Fa-f0-9]{4})([A-Fa-f0-9]{2})([A-Fa-f0-9]{2})\\)");
    private static final Pattern unknownPattern = Pattern.compile("\\(0x([A-Fa-f0-9]{1,8})\\)");
    private String name;
    private String data1;
    private String data2;
    private boolean useLocalUnit = true;
    private boolean localOnly = true;
    private boolean custom = false;
    private HashSet<Integer> sendToUnits;

    public NodoCommand(CommandInfo action, String d1, String d2) {
        this(action.toString(), d1, d2);
        this.localOnly = action.getType().is(CommandType.SERIALONLY);
    }

    private NodoCommand(String action, String d1, String d2) {
        this.name = action;
        this.data1 = d1;
        this.data2 = d2;
        this.sendToUnits = new HashSet<Integer>();

        CommandInfo info = CommandLoader.get(action);
        if (info != null) {
            this.localOnly = info.getType().is(CommandType.SERIALONLY);
        }
    }

    public NodoCommand(Device device) {
        this.name = device.getSignal();
    }

    public static NodoCommand fromString(String cmd) {
        // (Status Unit,0)
        // 
        Matcher matcher = elementPattern.matcher(cmd);
        if (matcher.matches()) {
            return new NodoCommand(matcher.group(1), matcher.group(2), matcher.group(3));
        }
        matcher = hexPattern.matcher(cmd);
        if (matcher.matches()) {
            return getCustomCommand(matcher.group(1), matcher.group(2), matcher.group(3));
        }
        matcher = unknownPattern.matcher(cmd);
        if (matcher.matches()) {
            return getCustomCommand(matcher.group(1), null, null);
        }
//        System.out.println("Could not parse: " + cmd);
        return null;
    }

    public static NodoCommand getStatusCommand(CommandInfo setting, int nodo) {
        return getStatusCommand(setting, -1, nodo);
    }

    public static NodoCommand getStatusCommand(CommandInfo setting, int item, int nodo) {
        CommandInfo status = CommandLoader.get(CommandInfo.Name.Status);
        NodoCommand cmd = new NodoCommand(status, setting == null ? "All" : setting.getName(), item < 0 ? null : String.valueOf(item));
        if ( nodo > 0 ) {
            cmd.setTargetNodo(nodo);
        }
        return cmd;
    }

    public static NodoCommand getCustomCommand(String cmd, String v1, String v2) {
        NodoCommand nc = new NodoCommand(cmd, v1, v2);
        nc.custom = true;
        // TODO: Ouch. I do not know if the created command is local or not.
        // Let the Nodo decide than...
        nc.localOnly = false;
//        System.out.println("Created CUSTOM with: " + nc.getName() + ":" + nc.getData1() + ":" + nc.getData2());
        return nc;
    }

    public boolean matches(Device device) {
        return name.equals(device.getSignal()) || toString().equals(device.getSignal());
    }

    public void setData(String one, String two) {
        if (one != null) {
            if (data1 != null) {
                throw new AccessControlException("Already set");
            }
            data1 = one;
        }
        if (two != null) {
            if (data2 != null) {
                throw new AccessControlException("Already set");
            }
            data2 = two;
        }
    }

    public boolean isCustom() {
        return custom;
    }

    public String getRawCommand() {
        StringBuilder builder = new StringBuilder();
        if (useLocalUnit || sendToUnits.isEmpty()) {
            builder.append(toString());
            builder.append(';');
        }
        if (!localOnly) {
            for (Integer unit : sendToUnits) {
                builder.append("Divert");
                builder.append(" ");
                builder.append(unit);
                builder.append("; ");
                builder.append(toString());
                builder.append(';');
            }
        }
        return builder.toString();
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        if (custom) {
            if (name != null && name.length() > 4) {
                return "0x" + name;
            }
            if (name == null) {
                builder.append("0x0000");
            } else {
                if (name.startsWith("0x")) {
                    builder.append("0000".substring(name.length() - 2));
                } else {
                    builder.append("0x");
                    builder.append("0000".substring(name.length()));
                }
                builder.append(name);
            }
            if (data1 == null) {
                builder.append("00");
            } else {
                builder.append("00".substring(data1.length()));
                builder.append(data1);
            }
            if (data2 == null) {
                builder.append("00");
            } else {
                builder.append("00".substring(data2.length()));
                builder.append(data2);
            }
        } else {
            builder.append(name);
            if (data1 != null && data1.trim().length() > 0) {
                builder.append(" ");
                builder.append(data1);
                if (data2 != null && data2.trim().length() > 0) {
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

    public void setTargetNodo(int unit) {
        useLocalUnit = false;
        sendToUnits.clear();
        sendToUnits.add(unit);
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
        useLocalUnit = options.isUseLocalUnit();
        sendToUnits.clear();

        if (options.isUseRemoteUnits()) {
            String[] list = options.getRemoteUnits();
            for (int i = 0; i < list.length; i++) {
                sendToUnits.add(Integer.parseInt(list[i]/*, 16*/));
            }
        }
    }
}
