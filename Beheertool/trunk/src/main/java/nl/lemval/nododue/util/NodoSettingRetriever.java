/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.util.listeners.OutputEventListener;

/**
 *
 * @author Michael
 */
public class NodoSettingRetriever {

    private NodoSettingRetriever() {}
    
    public static Collection<NodoSetting> getSettings(Collection<CommandInfo> cis) {
        String result = queryCommands(cis);
        if ( result == null )
            return null;
        return parseSettings(result, cis);
    }

    private static String queryCommands(Collection<CommandInfo> cis) {
        final StringBuilder builder = new StringBuilder();
        OutputEventListener listener = new OutputEventListener() {
            public void handleOutputLine(String message) {
                builder.append(message);
		builder.append(';');
            }
            public void handleClear() {}
        };
        SerialCommunicator comm =
            NodoDueManager.getApplication().getSerialCommunicator();
        if ( ! comm.isListening() ) {
            return null;
        }
        comm.addOutputListener(listener);

        for (CommandInfo ci : cis) {
            int[] range = ci.getQueryRange();
            if ( range.length > 0 ) {
                for (int i = 0; i < range.length; i++) {
                    comm.send(NodoCommand.getStatusCommand(ci, range[i]));
                    comm.waitCommand(200);
                }
            } else {
                comm.send(NodoCommand.getStatusCommand(ci));
                comm.waitCommand(200);
            }
        }
        // Need to sleep and wait for the response, since the Nodo cannot
        // handle all the commands at once. Waitcommand does not help, due
        // to the busy indicator can be reset by an earlier command.
        comm.waitCommand(2000);
        comm.removeOutputListener(listener);
        return builder.toString();
    }

    private static Collection<NodoSetting> parseSettings(String result, Collection<CommandInfo> cis) {
        HashMap<String, CommandInfo> map = new HashMap<String, CommandInfo>();
        Collection<NodoSetting> rv = new HashSet<NodoSetting>();
        for (CommandInfo commandInfo : cis) {
            map.put(commandInfo.getName(), commandInfo);
        }

	Pattern commandPattern = Pattern.compile("\\(([^\\)]+)\\)");
	Pattern elementPattern = Pattern.compile("\\(([A-Za-z]+) ?([^,; \\)]*),? ?([^,; \\)]*)\\)");

	Matcher matcher = commandPattern.matcher(result);

        while ( matcher.find() ) {
//            System.out.println("Match on: " + matcher.group());
	    Matcher data = elementPattern.matcher(matcher.group());
	    if (!data.matches()) {
		System.out.println("Oeps, no element match on '"+matcher.group()+"'...");
		continue;
	    }
            CommandInfo info = map.get(data.group(1));
            if ( info != null ) {
                NodoSetting setting = new NodoSetting(info);
		String data1 = data.group(2);
		String data2 = data.group(3);
//		System.out.println("Data: '" + data1 + "'" + data2 + "'");
		setting.setAttributeValue1(data1);
		setting.setAttributeValue2(data2);

                rv.add(setting);
            }
        }
        return rv;
    }

//    private static String valueOf(Collection<CommandInfo> cis) {
//        StringBuilder builder = new StringBuilder();
//        builder.append("[");
//        builder.append(cis.size());
//        builder.append("(");
//        for (CommandInfo commandInfo : cis) {
//            builder.append(commandInfo.toString());
//            builder.append(", ");
//        }
//        builder.replace(builder.length()-2, builder.length(), "");
//        builder.append(")]");
//        return builder.toString();
//    }
//
    public static void storeSettings(Collection<NodoSetting> settings) {
        SerialCommunicator comm =
            NodoDueManager.getApplication().getSerialCommunicator();
        if ( ! comm.isListening() ) {
            return;
        }
        Collection<CommandInfo> cis = CommandLoader.getActions(CommandType.COMMAND);
        HashMap<String, CommandInfo> map = new HashMap<String, CommandInfo>();
        for (CommandInfo commandInfo : cis) {
            map.put(commandInfo.getName(), commandInfo);
        }
        for (NodoSetting nodoSetting : settings) {
            CommandInfo cmd = map.get(nodoSetting.getName());
            if ( cmd != null ) {
                NodoCommand nc = new NodoCommand(cmd, nodoSetting.getAttributeData1(), nodoSetting.getAttributeData2());
                nc.makeDistributed();
                comm.send(nc);
                comm.waitCommand(150, 500);
            }
        }
    }

}
