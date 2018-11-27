package com.argcv.cite.service;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

import com.argcv.cite.model.CitationItem;
import com.google.gson.Gson;

public class CitationReader {
	private class CitationGroup{
		public List<CitationItem> ref;

		public List<CitationItem> getRef() {
			return ref;
		}
	}
	
	private static String host = "localhost";
	private static int port = 9529;
	private int timeout = 3000;
	private final static Integer DEFAULT_RETRY_TIMES = 3;

	private static CitationReader instance = null;

	private CitationReader() {
	}

	public synchronized static CitationReader getInstance() {
		return getInstance(host, port);
	}

	public synchronized static CitationReader getInstance(String host, int port) {
		if ((host != null && !host.trim().equals("") && !host.trim().equals(CitationReader.host)) || (port != CitationReader.port)) {
			CitationReader.host = host;
			CitationReader.port = port;
			instance = new CitationReader();
		} else if (instance == null) {
			instance = new CitationReader();
		}
		return instance;
	}
	
	public List<CitationItem> citeParse(String instr){
		return citeParse(instr,DEFAULT_RETRY_TIMES);
	}

	public List<CitationItem> citeParse(String instr , int retryTimes) {
		List<CitationItem> citeList = new ArrayList<CitationItem>();
		if (retryTimes <= 0) {
			retryTimes = 1;
		}
		boolean success = false;
		Gson g = new Gson();
		// Create a socket with a timeout
		while (retryTimes-- > 0 && !success) {
			Socket sock = null;
			try {
				InetAddress addr = InetAddress.getByName(host);
				SocketAddress sockaddr = new InetSocketAddress(addr, port);

				// Create an unbound socket
				sock = new Socket();

				// This method will block no more than timeoutMs.
				// If the timeout occurs, SocketTimeoutException is thrown.
				int timeoutMs = timeout; // 2 seconds
				sock.connect(sockaddr, timeoutMs);

				// submit the text content to the term extraction server
				BufferedWriter wr = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()));
				wr.write(instr + "#MTFBWU#");
				wr.flush();

				// read the extraction results
				BufferedReader rd = new BufferedReader(new InputStreamReader(sock.getInputStream()));
				StringBuffer sb = new StringBuffer();
				String str;
				while ((str = rd.readLine()) != null) {
					sb.append(str).append("\n");
				}
				//TODO convert
				CitationGroup rst = g.fromJson(sb.toString(), CitationGroup.class);
				citeList.addAll(rst.getRef());
				rd.close();
				success = true;
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (SocketTimeoutException e) {
				e.printStackTrace();
			} catch (Exception e) {
				StringBuilder sb = new StringBuilder();
				sb.append("[CitationReader] [E] ");
				sb.append(String.format("[Server:%s:%d, timeout: %d]", CitationReader.host, CitationReader.port, this.timeout));
				sb.append(", ").append(e.getLocalizedMessage()).append(".");
				System.err.println(sb.toString());
			}finally{
				if(sock != null ){
					try {
						sock.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}
		}
		return citeList;
	}
	
	public static void main(String [] args){
		File f = new File("test/f.txt");
		BufferedReader br = null;
		try {
			br = new BufferedReader(new FileReader(f));
			StringBuffer cb = new StringBuffer();
			String s;
			while((s = br.readLine()) != null){
				cb.append(s).append("\n");
			}
			List<CitationItem> cis = CitationReader.getInstance().citeParse(cb.toString());
			for(CitationItem ci : cis){
				System.out.println(ci);
			}
		} catch (IOException e) {
			e.printStackTrace();
		} finally{
			if(br != null){
				try {
					br.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

}
