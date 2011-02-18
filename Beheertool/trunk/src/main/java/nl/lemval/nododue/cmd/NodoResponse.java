/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.cmd;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import nl.lemval.nododue.cmd.CommandInfo.Name;

/**
 *
 * @author Michael
 */
public class NodoResponse {

    private static final DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm");
    private Source source;
    private Direction direction;
    private Date time;
    private NodoCommand command;

    public enum Direction {

        Output,
        Input,
        Unknown,
    }

    public enum Source {

        RF,
        IR,
        Serial,
        System,
    }

    public static NodoResponse[] getResponses(String message) {
        ArrayList<NodoResponse> result = new ArrayList<NodoResponse>();

        // Parsing depends on the Display setting !
        String[] responses = message.split("\r\n");
        for (int i = 0; i < responses.length; i++) {
            String[] responseData = responses[i].split(", ");
            NodoResponse response = new NodoResponse();
            for (int j = 0; j < responseData.length; j++) {
                String[] elemData = responseData[j].split("=");
                if (elemData.length == 1) {
                    if ( response.direction == null ) {
                        // This is an assumption, s
                        response.direction = Direction.Output;
                    }
                    response.parseSingle(elemData[0]);
                } else {
                    response.parseDouble(elemData);
                }
            }
            if ( response.getCommand() != null ) {
                result.add(response);
            }
        }
//        for (NodoResponse response : result) {
//            System.out.println("Response = " + response);
//        }
        return result.toArray(new NodoResponse[result.size()]);
    }

    private void parseDouble(String[] elemData) {
//        System.out.println("On message:  ");
//        for (String ed : elemData) {
//            System.out.println("Response element " + ed);
//        }
        if ("Source".equals(elemData[0])) {
            try {
                source = Source.valueOf(elemData[1]);
            } catch (Exception ex) {
                Logger.getLogger(NodoResponse.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        if ("Timestamp".equals(elemData[0])) {
            try {
                time = dateFormat.parse(elemData[1].substring(4));
            } catch (ParseException ex) {
                Logger.getLogger(NodoResponse.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        if ("Direction".equals(elemData[0])) {
            try {
                direction = Direction.valueOf(elemData[1]);
            } catch (Exception ex) {
                Logger.getLogger(NodoResponse.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        if ("Event".equals(elemData[0])) {
            command = NodoCommand.fromString(elemData[1]);
        }
    }

    private void parseSingle(String data) {
        try {
            direction = Direction.valueOf(data);
            return;
        } catch (Exception ex) {
            Logger.getLogger(NodoResponse.class.getName()).log(Level.FINE, null, ex);
        }
        try {
            source = Source.valueOf(data);
            return;
        } catch (Exception ex) {
            Logger.getLogger(NodoResponse.class.getName()).log(Level.FINE, null, ex);
        }
        command = NodoCommand.fromString(data);
    }

    public boolean is(Direction d) {
        return d == direction;
    }

    public boolean is(Source s) {
        return s == source;
    }

    public boolean is(Name name) {
        return (command != null && name.name().equals(command.getName()));
    }

    public NodoCommand getCommand() {
        return command;
    }

    public String getDirection() {
        if (direction != null) {
            return direction.name();
        }
        return Direction.Unknown.name();
    }

    @Override
    public String toString() {
        return "[" + source + "|" + direction + "|" + time + "] " + command;
    }
    
    
}
