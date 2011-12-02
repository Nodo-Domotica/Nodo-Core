/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.cmd.NodoMacroResponse;
import nl.lemval.nododue.cmd.NodoResponse;
import nl.lemval.nododue.util.listeners.OutputEventListener;

/**
 *
 * @author Michael
 */
public class NodoMacroHandler {

    public NodoMacroList getList() {
        CommandInfo command = CommandLoader.get(CommandInfo.Name.EventlistShow);
        NodoCommand nodoCommand = new NodoCommand(command, null, null);
        String content = execute(new NodoCommand[]{nodoCommand});
        return parseSettings(content);
    }

    private String execute(NodoCommand[] commands/*, boolean read*/) {
        final StringBuilder builder = new StringBuilder();
        final long[] lastUpdated = new long[1];
        
        OutputEventListener listener = new OutputEventListener() {

            public void handleOutputLine(String message) {
                builder.append(message);
                builder.append(System.getProperty("line.separator"));
                lastUpdated[0] = System.currentTimeMillis();
            }

            public void handleClear() {
            }

            public void handleNodoResponses(NodoResponse[] responses) {
            }
        };
        SerialCommunicator comm =
                NodoDueManager.getApplication().getSerialCommunicator();
        if (!comm.isListening()) {
            return null;
        }
        comm.addOutputListener(listener);
        for (int i = 0; i < commands.length; i++) {
            comm.send(commands[i]);
        }
        lastUpdated[0] = System.currentTimeMillis();
        while (lastUpdated[0] + 250 > System.currentTimeMillis()) {
            comm.waitCommand(150);
        }
        comm.removeOutputListener(listener);
        return builder.toString();
    }

    private NodoMacroList parseSettings(String result) {
        NodoMacroList list = new NodoMacroList();
        if (result == null) {
            return list;
        }

        NodoMacroResponse[] macros = NodoMacroResponse.getMacros(result);
        for (int i = 0; i < macros.length; i++) {
            NodoMacroResponse macro = macros[i];
            try {
                list.add(new NodoMacro(macro.getEvent(), macro.getAction()));
            } catch (Exception e) {
                NodoDueManager.showMessage("Macroloader.parse_failed", macro.getIndex(), result, e);
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
        NodoCommand[] commands = new NodoCommand[3 * list.size() + 1];
        commands[0] = new NodoCommand(CommandLoader.get(CommandInfo.Name.EventlistErase), null, null);

        CommandInfo write = CommandLoader.get(CommandInfo.Name.EventlistWrite);
        NodoCommand writeCmd = new NodoCommand(write, null, null);
        for (int i = 0; i < list.size(); i++) {
            final NodoMacro listItem = list.get(i);
            if (listItem != null) {
                commands[3 * i + 1] = writeCmd;
                commands[3 * i + 2] = listItem.getEvent();
                commands[3 * i + 3] = listItem.getAction();
            }
        }
        execute(commands);
        return true;
    }
}
