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
import nl.lemval.nododue.cmd.CommandInfo.Name;

/**
 *
 * @author Michael
 */
public class NodoResponse {

    private static final DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm");
    private Source source = Source.Unknown;
    private Direction direction = Direction.Unknown;
    private Date time;
    private NodoCommand command;
    private int unit;

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
        Unknown,
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
                    response.parseSingle(elemData[0]);
                } else {
                    response.parseDouble(elemData);
                }
            }
            if (response.getCommand() != null) {
                result.add(response);
            }
        }
        return result.toArray(new NodoResponse[result.size()]);
    }

    protected void parseDouble(String[] elemData) {
        if ("Source".equals(elemData[0])) {
            try {
                source = Source.valueOf(elemData[1]);
            } catch (Exception ex) {
            }
        }
        if ("TimeStamp".equals(elemData[0])) {
            try {
                time = dateFormat.parse(elemData[1].substring(4));
            } catch (ParseException ex) {
            }
        }
        if ("Direction".equals(elemData[0])) {
            try {
                direction = Direction.valueOf(elemData[1]);
            } catch (Exception ex) {
            }
        }
        if ("Event".equals(elemData[0])) {
            command = NodoCommand.fromString(elemData[1]);
        }
        if ("Unit".equals(elemData[0])) {
            unit = Integer.parseInt(elemData[1]);
        }
    }

    protected void parseSingle(String data) {
        try {
            direction = Direction.valueOf(data);
            return;
        } catch (Exception ex) {
        }
        try {
            source = Source.valueOf(data);
            return;
        } catch (Exception ex) {
        }
        try {
            unit = Integer.parseInt(data);
            return;
        } catch (Exception ex) {
        }
        try {
            time = dateFormat.parse(data.substring(4));
        } catch (Exception ex) {
        }
        command = NodoCommand.fromString(data);
    }

    public boolean is(Direction d) {
        return direction == Direction.Unknown || d == direction;
    }

    public boolean is(Source s) {
        return source == Source.Unknown || s == source;
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

    public int getUnit() {
        return unit;
    }

    @Override
    public String toString() {
        return "[" + source + "|" + direction + "|" + time + "] " + command;
    }

    protected void setCommand(NodoCommand cmd) {
        this.command = cmd;
    }
}
