/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.apache.commons.lang.StringUtils;

/**
 *
 * @author Michael
 */
public class Device implements Comparable<Device>{

    private static final Pattern pattern = Pattern.compile("INPUT: ([A-Z]+), \\(?([^\\)]*).*");

    public static Device parseFrom(String cmd) {
        Matcher matcher = pattern.matcher(cmd);
        if ( matcher.find() ) {
            Device device = new Device(matcher.group(2));
            device.setSource(matcher.group(1));
            return device;
        }
        return null;
    }

    private String signal;
    private String name;
    private String location;
    private String source;
    private boolean ignored;
    private static final char SEPARATOR = '|';

    public Device(String signal) {
        this.signal = signal.replace(", ", ",");
        this.name = "";
        this.location = "";
        this.ignored = false;
    }

    public static Device fromString(String dv) {
        Device d = new Device("onbekend");
        if (d.parse(dv)) {
            return d;
        }
        return null;
    }

    public String getLocation() {
        return location;
    }

    public void setLocation(String location) {
        this.location = location;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getSignal() {
        return signal;
    }

    public void setSignal(String signal) {
        this.signal = signal;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public String getSource() {
        return source;
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
        if ( isIgnored() ) {
            builder.append(SEPARATOR);
            builder.append("true");
        }
        return builder.toString();
    }

    public boolean isIgnored() {
        return ignored;
    }

    public void setIgnored(boolean ignored) {
        this.ignored = ignored;
    }


    private boolean parse(String value) {
        try {
            // SEPARATOR
            String segments[] = value.split("\\|");
            setSignal(segments[0]);
            setName(segments[1]);
            setLocation(segments[2]);
            setSource(segments[3]);
            if ( segments.length > 4 ) { setIgnored(Boolean.parseBoolean(segments[4])); }
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    public String toString() {
        return signal;
    }

    @Override
    public boolean equals(Object obj) {
        if ( obj instanceof Device ) {
            return signal.equals(((Device) obj).signal);
        }
        return false;
    }

    public int compareTo(Device o) {
        return signal.compareTo(o.signal);
    }

    public boolean isActive() {
        return !isIgnored() && StringUtils.isNotBlank(name);
    }
}
