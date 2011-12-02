/*
 * NodoDueManager.java
 */
package nl.lemval.nododue;

import gnu.io.RXTXVersion;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.text.MessageFormat;
import java.util.ResourceBundle;

/**
 * The main class of the application.
 */
public class NodoDueManagerStarter {

    private static ResourceBundle bundle = ResourceBundle.getBundle("application");
    private static boolean debug = false;

    /**
     * Main method launching the application.
     */
    public static void main(String[] args) {

        File commPath = null;
        String arch = null;

        for (int i = 0; i < args.length; i++) {
            if ( "-debug".equals(args[i]) ) {
                debug = true;
            } else if ( "-comm".equals(args[i]) && args.length > i+1) {
                commPath = new File(args[i+1]);
            } else if ( "-arch".equals(args[i]) && args.length > i+1) {
                arch = args[i+1];
            }
        }

        if ( commPath == null ) {
            if ( arch ==  null ) {
                arch = getSystemPostfix();
                if ( debug ) {
                    System.out.println("Architecture = " + arch);
                }
            }
            String sep = System.getProperty("file.separator");
            StringBuilder builder = new StringBuilder();
            builder.append(System.getProperty("user.dir"));
            builder.append(sep);
            builder.append("lib");
            builder.append(sep);
            builder.append(arch);
            commPath = new File(builder.toString());
        }

        try {
            loadLibraries(commPath);
        } catch (Exception e) {
            System.out.println(MessageFormat.format(bundle.getString("run.fix_path_failed"), commPath));
            if ( debug ) {
                e.printStackTrace();
            }
            System.out.println(bundle.getString("run.terminate"));
            System.out.println(bundle.getString("run.64bit_message"));
            return;
        }

        NodoDueManager.launch(NodoDueManager.class, args);
    }

    private static String getSystemPostfix() {
        String osName = System.getProperty("os.name").toLowerCase();
        String osArch = System.getProperty("os.arch").toLowerCase();

        if (osName.contains("windows")) {
            if (osArch.contains("64")) {
                return "win64";
            }
            return "win32";
        }

        if (osName.contains("linux")) {
            NodoDueManager.linux = true;
            if (osArch.contains("64")) {
                return "linux64";
            }
            return "linux32";
        }

        if (osName.contains("mac")) {
            return "mac";
        }

        System.out.println(MessageFormat.format(bundle.getString("run.no_architecture"), osName, osArch));
        System.out.println(bundle.getString("run.terminate"));
        System.exit(-1);
        return null;
    }

    private static void loadLibraries(File commPath) throws Exception {
        // A lot of code to fix the java.library.path to load the correct
        // native I/O library...
        //

        // Reset the "sys_paths" field of the ClassLoader to null.
        Class clazz = ClassLoader.class;
        Field field = clazz.getDeclaredField("sys_paths");
        boolean accessible = field.isAccessible();
        if (!accessible) {
            field.setAccessible(true);
        }
        Object original = field.get(clazz);
        // Reset it to null so that whenever "System.loadLibrary" is called,
        // it will be reconstructed with the changed value.
        field.set(clazz, null);
        try {
            // Change the value and load the library.
            System.setProperty("java.library.path",
                commPath.getPath() +
                System.getProperty("path.separator") +
                System.getProperty("java.library.path"));
            System.loadLibrary("rxtxSerial");
        } catch (Throwable t) {
            throw new Exception(t);
        } finally {
            // Revert back the changes.
            field.set(clazz, original);
            field.setAccessible(accessible);
        }

            File[] jarFiles = commPath.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.endsWith(".jar");
            }
        });

        URLClassLoader sysloader = (URLClassLoader) ClassLoader.getSystemClassLoader();
        try {
            Method method = URLClassLoader.class.getDeclaredMethod("addURL", new Class[] {URL.class});
            method.setAccessible(true);
            for (File jarFile : jarFiles) {
                URL url = jarFile.toURI().toURL();
                if ( debug ) {
                    System.out.println("Adding url: " + url);
                }
                method.invoke(sysloader, new Object[] {url});
            }
            NodoDueManagerStarter.class.getClassLoader().loadClass("gnu.io.RXTXVersion");
        } catch (Throwable t) {
            if ( debug ) {
                t.printStackTrace();
            }
            throw new IOException("Error, could not add URL to system classloader");
        }

        System.out.println("RXTX Java versie   : " +  RXTXVersion.getVersion());
        System.out.println("RXTX systeem versie: " +  RXTXVersion.nativeGetVersion());
    }
}
