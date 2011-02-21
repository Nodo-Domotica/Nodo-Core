/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.cmd.NodoResponse;
import nl.lemval.nododue.util.listeners.OutputEventListener;

/**
 *
 * @author Michael
 */
public class NodoSettingRetriever {

    private NodoSettingRetriever() {
    }

    public static Collection<NodoSetting> getAllSettings() {
        Collection<CommandInfo> cis = CommandLoader.getActions(CommandType.ALL);
//        long t = System.currentTimeMillis();
        ArrayList<NodoResponse> result = queryCommands(null, 0);
//        System.out.println("Querying took " + (System.currentTimeMillis()-t) + "ms.");
        return parseSettings(result, cis);
    }

    public static Collection<NodoSetting> getSettings(Collection<CommandInfo> cis, int nodo) {
        // If there is full display, this is quicker than all settings.
        // If there is minimal display, this is slower on 20+ settings.
//        long t = System.currentTimeMillis();
        ArrayList<NodoResponse> result = queryCommands(cis, nodo);
//        System.out.println("Querying took " + (System.currentTimeMillis()-t) + "ms.");
        return parseSettings(result, cis);
    }

    private static ArrayList<NodoResponse> queryCommands(Collection<CommandInfo> cis, int nodo) {
//        final StringBuilder builder = new StringBuilder();
        final ArrayList<NodoResponse> result = new ArrayList<NodoResponse>();
        final long[] inputReceived = new long[1];
        inputReceived[0] = System.currentTimeMillis();
        OutputEventListener listener = new OutputEventListener()    {

            public void handleOutputLine(String message) {
                inputReceived[0] = System.currentTimeMillis();
            }

            public void handleClear() {
            }

            public void handleNodoResponses(NodoResponse[] responses) {
                for (NodoResponse nodoResponse : responses) {
                    if (nodoResponse.is(NodoResponse.Direction.Output)) {
                        result.add(nodoResponse);
                    }
                }
            }
        };
        SerialCommunicator comm =
                NodoDueManager.getApplication().getSerialCommunicator();
        if (!comm.isListening()) {
            return null;
        }
        comm.addOutputListener(listener);

        if (cis == null) {
            NodoCommand statusCommand = NodoCommand.getStatusCommand(null, nodo);
            comm.send(statusCommand);
        } else {
            for (CommandInfo ci : cis) {
                int[] range = ci.getQueryRange();
                if (range.length > 0) {
                    for (int i = 0; i < range.length; i++) {
                        final NodoCommand statusCommand = NodoCommand.getStatusCommand(ci, range[i], nodo);
                        comm.send(statusCommand);
                        comm.waitCommand();
                    }
                } else {
                    final NodoCommand statusCommand = NodoCommand.getStatusCommand(ci, nodo);
                    comm.send(statusCommand);
                    comm.waitCommand();
                }
            }
        }
        while (inputReceived[0] + 250 > System.currentTimeMillis()) {
            comm.waitCommand(250);
        }

        // Need to sleep and wait for the response, since the Nodo cannot
        // handle all the commands at once. Waitcommand does not help, due
        // to the busy indicator can be reset by an earlier command.
        comm.removeOutputListener(listener);
        return result;
    }

    private static Collection<NodoSetting> parseSettings(ArrayList<NodoResponse> responses, Collection<CommandInfo> cis) {
        HashMap<String, CommandInfo> map = new HashMap<String, CommandInfo>();
        Collection<NodoSetting> rv = new HashSet<NodoSetting>();
        for (CommandInfo commandInfo : cis) {
            map.put(commandInfo.getName(), commandInfo);
        }
        for (NodoResponse nodoResponse : responses) {
            NodoCommand cmd = nodoResponse.getCommand();
            if (cmd != null) {
                CommandInfo info = map.get(cmd.getName());
                if (info != null) {
                    NodoSetting setting = new NodoSetting(info);
                    setting.setAttributeValue1(cmd.getData1());
                    setting.setAttributeValue2(cmd.getData2());

                    rv.add(setting);
                }
            }
        }
        return rv;
    }

    public static void storeSettings(Collection<NodoSetting> settings, int unit) {
        SerialCommunicator comm =
                NodoDueManager.getApplication().getSerialCommunicator();
        if (!comm.isListening()) {
            return;
        }
        Collection<CommandInfo> cis = CommandLoader.getActions(CommandType.COMMAND);
        HashMap<String, CommandInfo> map = new HashMap<String, CommandInfo>();
        for (CommandInfo commandInfo : cis) {
            map.put(commandInfo.getName(), commandInfo);
        }
        for (NodoSetting nodoSetting : settings) {
            CommandInfo cmd = map.get(nodoSetting.getName());
            if (cmd != null) {
                NodoCommand nc = new NodoCommand(cmd, nodoSetting.getAttributeData1(), nodoSetting.getAttributeData2());
                if (unit > 0) {
                    nc.setTargetNodo(unit);
                }
                comm.send(nc);
                comm.waitCommand(50, 500);
            }
        }
    }
}
