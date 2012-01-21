/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.cmd.NodoResponse;
import org.apache.commons.lang.StringUtils;

/**
 *
 * @author Michael
 */
public class Device implements Comparable<Device> {
    
    private static final char SEPARATOR = '|';
    private String signal;
    private String name;
    private String location;
    private String source;
    private boolean ignored;
    
    public Device(String signal) {
        this.signal = signal.replace(", ", ",");
        this.name = "";
        this.location = "";
        this.ignored = false;
    }
    
    public static Device parseFrom(String cmd) {
        NodoResponse[] responses = NodoResponse.getResponses(cmd);
        for (NodoResponse nodoResponse : responses) {
            NodoCommand command = nodoResponse.getCommand();
            if (command != null) {
                CommandInfo get = CommandLoader.get(command.getName());
//                System.out.println("On " + command.getName() + ": " + get);
                if (command.isCustom() || (get != null && get.getType().is(CommandType.DEVICE))) {
//                    System.out.println("Registering new device: " + nodoResponse);
                    return new Device(command.toString());
                }
            }
        }
        return null;
    }
    
    public String getLocation() {
        return location;
    }
    
    public void setLocation(String location) {
        if (StringUtils.isNotBlank(location) && StringUtils.isBlank(this.location)) {
            this.location = location;
        }
    }
    
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        if (StringUtils.isNotBlank(name) && StringUtils.isBlank(this.name)) {
            this.name = name;
        }
    }
    
    public String getSignal() {
        return signal;
    }
    
    public void setSignal(String signal) {
        if (StringUtils.isNotBlank(signal) && StringUtils.isBlank(this.signal)) {
            this.signal = signal;
        }
    }
    
    public void setSource(String source) {
        if (StringUtils.isNotBlank(source) && StringUtils.isBlank(this.source)) {
            this.source = source;
        }
    }
    
    public String getSource() {
        return source;
    }
    
    public boolean isIgnored() {
        return ignored;
    }
    
    public void setIgnored(boolean ignored) {
        this.ignored = ignored;
    }
    
    public static Device fromString(String value) {
        Device result = null;
        try {
            // SEPARATOR
            String segments[] = value.split("\\|");
            result = new Device(segments[0]);
            result.setName(segments[1]);
            result.setLocation(segments[2]);
            result.setSource(segments[3]);
            if (segments.length > 4) {
                result.setIgnored(Boolean.parseBoolean(segments[4]));
            }
        } catch (Exception e) {
            return null;
        }
        return result;
    }
    
    public String asString() {
        StringBuilder builder = new StringBuilder();
        builder.append(signal);
        builder.append(SEPARATOR);
        builder.append(name);
        builder.append(SEPARATOR);
        builder.append(location);
        builder.append(SEPARATOR);
        builder.append(source);
        if (isIgnored()) {
            builder.append(SEPARATOR);
            builder.append("true");
        }
        return builder.toString();
    }
    
    @Override
    public String toString() {
        return signal;
    }
    
    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Device) {
            return signal.equals(((Device) obj).signal);
        }
        return false;
    }
    
    @Override
    public int hashCode() {
        int hash = 7;
        hash = 37 * hash + (this.signal != null ? this.signal.hashCode() : 0);
        return hash;
    }
    
    public int compareTo(Device o) {
        return signal.compareTo(o.signal);
    }
    
    public boolean isActive() {
        return !isIgnored() && StringUtils.isNotBlank(name);
    }
}
