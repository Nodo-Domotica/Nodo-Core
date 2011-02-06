/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue;

import nl.lemval.nododue.util.InputRange;
import java.awt.Font;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import nl.lemval.nododue.util.Device;
import org.apache.commons.configuration.PropertiesConfiguration;

/**
 *
 * @author Michael
 */
public class Options {

    private static Options instance = null;

    private PropertiesConfiguration config;
    private boolean hasScanned = false;
    private HashMap<String, Device> devices;

    private InputRange defaultRange;

    enum Opts {
        // short

        nodoUnit, nodoHome,
        // String array
        remoteUnits, devices, inputRanges,
        // boolean
        useLocalUnit, useRemoteUnits, nodoUnitAutoScan, scanResponses,
        // String
        lastComportUsed, folder, domesticCode, baseFont, titleFont,
        // int
        lastBaudrateUsed, maxHistorySize,
    }

    private Options() {
        devices = new HashMap<String, Device>();
        defaultRange = new InputRange("Bitwaarde", 0, 255, 1, null, 0);
    }

    public static Options getInstance() {
        if (instance == null) {
            instance = new Options();
            instance.load();
        }
        return instance;
    }

    public Object[] getInputRanges() {
        ArrayList<InputRange> ranges = new ArrayList<InputRange>();
        ranges.add(defaultRange);
        String[] stringArray = config.getStringArray(Opts.inputRanges.name());
        for (String data : stringArray) {
            InputRange range = InputRange.fromString(data);
            if (range != null) {
                ranges.add(range);
            }
        }
        return ranges.toArray();
    }

    private void load() {
        try {
            config = new PropertiesConfiguration("nododue.ini");

            String[] stringArray = config.getStringArray(Opts.devices.name());
            for (String dv : stringArray) {
                Device d = Device.fromString(dv.replace("~~", ","));
                if (d != null) {
                    devices.put(d.getSignal(), d);
                }
            }
            config.clearProperty(Opts.devices.name());

        } catch (Exception ex) {
            config = new PropertiesConfiguration();
        }
    }

    /*package*/ void save() {
        try {
            for (Device device : devices.values()) {
                config.addProperty(Opts.devices.name(), device.asString().replace(",", "~~"));
            }
            config.save("nododue.ini");
        } catch (Exception e) {
            NodoDueManager.showMessage("Configuration.save_failed", e.getMessage());
        }
    }

    public boolean hasScanned() {
        return hasScanned;
    }

    /**
     * Command to scan the Home and Unit parameters from the boot
     * string.
     * @param message
     */
    public boolean scanLine(String message) {
        Pattern info = Pattern.compile("Unit ([0-9]{1,2}), ([0-9]{1,2})|Unit ([0-9]{1,2}), Home ([0-9]{1,2})");
        Matcher m = info.matcher(message);

        if (m.find()) {
            setNodoUnit(m.group(1) == null ? m.group(4) : m.group(1));
            setNodoHome(m.group(2) == null ? m.group(3) : m.group(2));
            hasScanned = true;
        }
        return hasScanned;
    }

    public void registerResponse(String cmd) {
        if ( isScanResponse() ) {
            Device device = Device.parseFrom(cmd);
            if ( device != null ) {
                addAppliance(device);
            }
        }
    }

    public static String getFontString(Font f) {
        StringBuilder b = new StringBuilder();
        b.append(f.getFamily());
        b.append(" ");
        if (f.isBold()) {
            if (f.isItalic()) {
                b.append("BoldItalic");
            } else {
                b.append("Bold");
            }
        } else if (f.isItalic()) {
            b.append("Italic");
        } else {
            b.append("Plain");
        }
        b.append(" ");
        b.append(f.getSize());
        return b.toString();
    }

    public short getNodoUnit() {
        return config.getShort(Opts.nodoUnit.name(), (short) 0);
    }

    public void setNodoUnit(short nodoUnit) {
        config.setProperty(Opts.nodoUnit.name(), nodoUnit);
    }

    public boolean setNodoUnit(String nodoUnit) {
        try {
            setNodoUnit(Short.parseShort(nodoUnit));
        } catch (NumberFormatException e) {
            try {
                setNodoUnit(Short.parseShort(nodoUnit, 16));
            } catch (NumberFormatException e2) {
                return false;
            }
        }
        return true;
    }

    public short getNodoHome() {
        return config.getShort(Opts.nodoHome.name(), (short) 0);
    }

    public void setNodoHome(short nodoHome) {
        config.setProperty(Opts.nodoHome.name(), nodoHome);
    }

    public boolean setNodoHome(String nodoHome) {
        try {
            setNodoHome(Short.parseShort(nodoHome));
        } catch (NumberFormatException e) {
            try {
                setNodoHome(Short.parseShort(nodoHome, 16));
            } catch (NumberFormatException e2) {
                return false;
            }
        }
        return true;
    }

    public boolean isNodoUnitAutoScan() {
        return config.getBoolean(Opts.nodoUnitAutoScan.name(), true);
    }

    public void setNodoUnitAutoScan(boolean nodoUnitAutoScan) {
        config.setProperty(Opts.nodoUnitAutoScan.name(), nodoUnitAutoScan);
    }

    public int getLastBaudrateUsed() {
        return config.getInt(Opts.lastBaudrateUsed.name(), 19200);
    }

    public void setLastBaudrateUsed(int lastBaudrateUsed) {
        config.setProperty(Opts.lastBaudrateUsed.name(), lastBaudrateUsed);
    }

    public int getMaxHistorySize() {
        return config.getInt(Opts.maxHistorySize.name(), 5000);
    }

    public void setMaxHistorySize(int maxHistorySize) {
        config.setProperty(Opts.maxHistorySize.name(), maxHistorySize);
    }

    public String getLastComportUsed() {
        return config.getString(Opts.lastComportUsed.name());
    }

    public void setLastComportUsed(String lastComportUsed) {
        config.setProperty(Opts.lastComportUsed.name(), lastComportUsed);
    }

    public String[] getRemoteUnits() {
        return config.getStringArray(Opts.remoteUnits.name());
    }

    public void setRemoteUnits(Set<String> rus) {
        config.setProperty(Opts.remoteUnits.name(), rus.toArray(new String[rus.size()]));
    }

    public boolean isUseLocalUnit() {
        return config.getBoolean(Opts.useLocalUnit.name(), true);
    }

    public void setUseLocalUnit(boolean b) {
        config.setProperty(Opts.useLocalUnit.name(), b);
    }

    public boolean isUseRemoteUnits() {
        return config.getBoolean(Opts.useRemoteUnits.name(), false);
    }

    public void setUseRemoteUnits(boolean useRemoteUnits) {
        config.setProperty(Opts.useRemoteUnits.name(), useRemoteUnits);
    }

    public boolean isScanResponse() {
        return config.getBoolean(Opts.scanResponses.name(), false);
    }

    public void setScanResponse(boolean scanResponse) {
        config.setProperty(Opts.scanResponses.name(), scanResponse);
    }

    public String getFolder() {
        return config.getString(Opts.folder.name());
    }

    public void setFolder(String folder) {
        config.setProperty(Opts.folder.name(), folder);
    }

    public String getDomesticCode() {
        return config.getString(Opts.domesticCode.name());
    }

    public void setDomesticCode(String value) {
        config.setProperty(Opts.domesticCode.name(), value);
    }

    public Font getBaseFont() {
        String value = config.getString(Opts.baseFont.name(), "Tahoma Plain 11");
        return Font.decode(value);
    }

    public void setBaseFont(Font baseFont) {
        config.setProperty(Opts.baseFont.name(), getFontString(baseFont));
    }

    public Font getTitleFont() {
        String value = config.getString(Opts.titleFont.name(), "Tahoma Bold 15");
        return Font.decode(value);
    }

    public void setTitleFont(Font titleFont) {
        config.setProperty(Opts.titleFont.name(), getFontString(titleFont));
    }

    public void addAppliance(Device device) {
        devices.put(device.getSignal(), device);
    }

    public void removeAppliance(String code) {
        devices.remove(code);
    }

    public Set<Device> getApplicances() {
        return new TreeSet<Device>(devices.values());
    }

    public Device getAppliance(String code) {
        return devices.get(code);
    }
}
