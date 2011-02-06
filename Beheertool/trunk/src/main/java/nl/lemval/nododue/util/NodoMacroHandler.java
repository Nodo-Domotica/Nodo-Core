/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util;

import java.util.Collection;
import java.util.HashMap;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.util.listeners.OutputEventListener;

/**
 *
 * @author Michael
 */
public class NodoMacroHandler {

    public NodoMacroList getList() {
        CommandInfo command = CommandLoader.get(CommandInfo.Name.EventlistShow);
        String content = query(command);
        return parseSettings(content, CommandLoader.getActions(CommandType.ALL));
    }

    private String query(CommandInfo cmd) {
        return execute(new NodoCommand[] {new NodoCommand(cmd, null, null)}, true);
    }

    private String execute(NodoCommand[] commands, boolean read) {
        final StringBuilder builder = new StringBuilder();
        OutputEventListener listener = new OutputEventListener() {
            public void handleOutputLine(String message) {
                builder.append(message);
            }
            public void handleClear() {}
        };
        SerialCommunicator comm =
            NodoDueManager.getApplication().getSerialCommunicator();
        if ( ! comm.isListening() ) {
            return null;
        }
        comm.addOutputListener(listener);
        for (int i = 0; i < commands.length; i++) {
            comm.send(commands[i]);
            comm.waitCommand(100);
        }
	while ( read && builder.toString().endsWith("****") == false ) {
	    comm.waitCommand(1000);
	}
        comm.removeOutputListener(listener);
        return builder.toString();
    }

    private NodoMacroList parseSettings(String result, Collection<CommandInfo> cis) {
        NodoMacroList list = new NodoMacroList();
        if ( result == null ) {
            return list;
        }

        HashMap<String, CommandInfo> map = new HashMap<String, CommandInfo>();
        for (CommandInfo commandInfo : cis) {
            map.put(commandInfo.getName(), commandInfo);
        }
        Set<Device> applicances = Options.getInstance().getApplicances();
        HashMap<String, Device> dev = new HashMap<String, Device>();
        for (Device device : applicances) {
            if ( device.isActive() ) {
                dev.put(device.getSignal(), device);
            }
        }

        // EVENTLIST: 4: (Wildcard 0,0); (SendIR 5,0)
        Pattern pattern = Pattern.compile("([0-9]+): \\(?([^\\)]+)\\)?; \\(?([^\\)]+)\\)?");
        Matcher matcher = pattern.matcher(result);

        while ( matcher.find() ) {
            int index = Integer.parseInt( matcher.group(1) );
            try {
                list.add(index, NodoMacro.loadFrom(map, dev, matcher.group(2), matcher.group(3)));
            } catch (Exception e) {
                NodoDueManager.showMessage("Macroloader.parse_failed", index, result, e);
            }
        }
        return list;
    }

    /**
     * Writes the macro list in memory to the nodo. Each macro corresponds
     * to three commands. The action is preceded with an erase of all macros.
     *
     * @param list The list in memory with all macro's
     * @return
     */
    public Boolean writeList(NodoMacroList list) {
        NodoCommand[] commands = new NodoCommand[3*list.size()+1];
        commands[0] = new NodoCommand(CommandLoader.get(CommandInfo.Name.EventlistErase), null, null);

        CommandInfo write = CommandLoader.get(CommandInfo.Name.EventlistWrite);
        NodoCommand writeCmd = new NodoCommand(write, null, null);
        for (int i = 0; i < list.size(); i++) {
            commands[3*i+1] = writeCmd;
            commands[3*i+2] = list.get(i).getEvent();
            commands[3*i+3] = list.get(i).getAction();
        }

        // TODO : Appliances added are not parsed correctly and results in an empty entry in the list.
        // This nullpointers above....
        
        String result = execute(commands, false);
//        System.out.println("Result = " + result);
        return true;
    }
}
