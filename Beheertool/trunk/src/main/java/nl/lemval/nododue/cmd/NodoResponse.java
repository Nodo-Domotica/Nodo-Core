/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import java.util.ArrayList;

/**
 *
 * @author Michael
 */
public class NodoResponse {

    private Source source;
    private Direction direction;

    public enum Direction {
        OUTPUT,
        INPUT,
    }
    public enum Source {
        RF,
        IR,
        Serial,
        System,
    }

    public static NodoResponse[] getResponses(String message) {
        ArrayList<NodoResponse> result = new ArrayList<NodoResponse>();
        
        String[] responses = message.split("\r\n");
        for (int i = 0; i < responses.length; i++) {
            String[] responseData = responses[i].split(",");
            NodoResponse response = new NodoResponse();
            for (int j = 0; j < responseData.length; j++) {
                String[] elemData = responseData[j].split("=");
                if ( "Port".equals(elemData[0])) { response.source = Source.valueOf(elemData[1]); }
                // TODO
            }
            
        }
//        Pattern info = Pattern.compile("Unit ([0-9]{1,2}), ([0-9]{1,2})|Unit ([0-9]{1,2}), Home ([0-9]{1,2})");
//        Matcher m = info.matcher(message);
//
//        if (m.find()) {
//        }
        return result.toArray(new NodoResponse[result.size()]);
    }
    
    public boolean is(Direction d) {
        return d == direction;
    }

    public boolean is(Source s) {
        return s == source;
    }

    public boolean isCommand(String s) {
        throw new UnsupportedOperationException("Not yet implemented");
    }
    
    public String getFirstParameter() {
        throw new UnsupportedOperationException("Not yet implemented");
    }
}
