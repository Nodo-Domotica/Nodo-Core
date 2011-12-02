/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.cmd;

import java.util.ArrayList;
import nl.lemval.nododue.util.NodoMacro;

/**
 *
 * @author Michael
 */
public class NodoMacroResponse extends NodoResponse {

    private int index;
    private NodoCommand action;

    public NodoMacroResponse() {
    }

    public NodoMacroResponse(int index, NodoMacro nodoMacro) {
        this.index = index;
        setCommand(nodoMacro.getEvent());
        this.action = nodoMacro.getAction();
    }

    public static NodoMacroResponse[] getMacros(String message) {
        ArrayList<NodoMacroResponse> result = new ArrayList<NodoMacroResponse>();

        // Parsing depends on the Display setting !
        String[] responses = message.split("\r\n");
        for (int i = 0; i < responses.length; i++) {
            String[] responseData = responses[i].split(", ");
            NodoMacroResponse response = new NodoMacroResponse();
            for (int j = 0; j < responseData.length; j++) {
                String[] elemData = responseData[j].split("=");
                if (elemData.length == 1) {
                    response.parseSingle(elemData[0]);
                } else {
                    response.parseDouble(elemData);
                }
            }
            if (response.getEvent() != null && response.getAction() != null) {
                result.add(response);
            }
        }
        return result.toArray(new NodoMacroResponse[result.size()]);
    }

    @Override
    protected void parseSingle(String data) {
        // This hides parsing the unit, but this is irrelevant for events.
        try {
            index = Integer.parseInt(data);
            return;
        } catch (Exception ex) {
        }

        // Event and action are the same, but in order.
        if (getCommand() != null && action == null) {
            action = NodoCommand.fromString(data);
            if (action != null) {
                return;
            }
        }
        super.parseSingle(data);
    }

    @Override
    protected void parseDouble(String[] elemData) {
        if ("Line".equals(elemData[0])) {
            index = Integer.parseInt(elemData[1]);
        }
        if ("Action".equals(elemData[0])) {
            action = NodoCommand.fromString(elemData[1]);
        }
        super.parseDouble(elemData);
    }

    public NodoCommand getEvent() {
        return getCommand();
    }

    public NodoCommand getAction() {
        return action;
    }

    public int getIndex() {
        return index;
    }

    @Override
    public String toString() {
        return getIndex() + ", (" + getEvent() + "), (" + getAction() + ")";
    }
}
