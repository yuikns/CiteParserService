package com.argcv.cite.model;

import java.util.List;

public class CitationItem {
	List<String> authors;
	List<String> editors;
	String title;
	String journal;
	String booktitle;
	String publisher;
	String institution;
	String volume;
	String location;
	String pages;
	String date;
	String url;
	String isbn;
	String tech;

	public CitationItem() {
//		authors = new ArrayList<String>();
//		editors = new ArrayList<String>();
//		title = "";
//		journal = "";
//		booktitle = "";
//		publisher = "";
//		institution = "";
//		volume = "";
//		location = "";
//		pages = "";
//		date = "";
//		url = "";
//		isbn = "";
//		tech = "";
	}

	public List<String> getAuthors() {
		return authors;
	}

	public void setAuthors(List<String> authors) {
		this.authors = authors;
	}

	public List<String> getEditors() {
		return editors;
	}

	public void setEditors(List<String> editors) {
		this.editors = editors;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getJournal() {
		return journal;
	}

	public void setJournal(String journal) {
		this.journal = journal;
	}

	public String getBooktitle() {
		return booktitle;
	}

	public void setBooktitle(String booktitle) {
		this.booktitle = booktitle;
	}

	public String getPublisher() {
		return publisher;
	}

	public void setPublisher(String publisher) {
		this.publisher = publisher;
	}

	public String getInstitution() {
		return institution;
	}

	public void setInstitution(String institution) {
		this.institution = institution;
	}

	public String getVolume() {
		return volume;
	}

	public void setVolume(String volume) {
		this.volume = volume;
	}

	public String getLocation() {
		return location;
	}

	public void setLocation(String location) {
		this.location = location;
	}

	public String getPages() {
		return pages;
	}

	public void setPages(String pages) {
		this.pages = pages;
	}

	public String getDate() {
		return date;
	}

	public void setDate(String date) {
		this.date = date;
	}

	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

	public String getIsbn() {
		return isbn;
	}

	public void setIsbn(String isbn) {
		this.isbn = isbn;
	}

	public String getTech() {
		return tech;
	}

	public void setTech(String tech) {
		this.tech = tech;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "CitationItem \n[\n\t" + (authors != null ? "authors=" + authors + "\n\t" : "") + (editors != null ? "editors=" + editors + "\n\t" : "") + (title != null ? "title=" + title + "\n\t" : "") + (journal != null ? "journal=" + journal + "\n\t" : "") + (booktitle != null ? "booktitle=" + booktitle + "\n\t" : "") + (publisher != null ? "publisher=" + publisher + "\n\t" : "") + (institution != null ? "institution=" + institution + "\n\t" : "") + (volume != null ? "volume=" + volume + "\n\t" : "") + (location != null ? "location=" + location + "\n\t" : "") + (pages != null ? "pages=" + pages + "\n\t" : "") + (date != null ? "date=" + date + "\n\t" : "") + (url != null ? "url=" + url + "\n\t" : "") + (isbn != null ? "isbn=" + isbn + "\n\t" : "") + (tech != null ? "tech=" + tech : "") + "\r]";
	}

	
	
}
