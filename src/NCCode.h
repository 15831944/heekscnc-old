// NCCode.h

// This is an object that can go in the tree view as a child of a program.
// It contains lists of NC Code blocks, each one is a line of an NC file, but also contains drawing items
// The text of this is shown in the "output" window.

#pragma once

#include "../../interface/HeeksObj.h"
#include "../../interface/HeeksColor.h"
#include "HeeksCNCTypes.h"

enum TextColorEnum{
	TextColorDefaultType,
	TextColorBlockType,
	TextColorMiscType,
	TextColorProgramType,
	TextColorToolType,
	TextColorCommentType,
	TextColorVariableType,
	TextColorPrepType,
	TextColorAxisType,
	TextColorRapidType,
	TextColorFeedType,
	MaxTextColorTypes
};

enum LinesColorEnum{
	LinesColorRapidType,
	LinesColorFeedType,
	MaxLinesColorType
};

class ColouredText
{
public:
	wxString m_str;
	TextColorEnum m_color_type;
	ColouredText():m_color_type(TextColorDefaultType){}

	void WriteXML(TiXmlNode *root);
	void ReadFromXMLElement(TiXmlElement* pElem);
};

class PathObject{
public:
	double m_x[3];
	PathObject(){m_x[0] = m_x[1] = m_x[2] = 0.0;}
	virtual int GetType() = 0; // 0 - line, 1 - arc
	virtual void WriteXML(TiXmlNode *root) = 0;
	virtual void ReadFromXMLElement(TiXmlElement* pElem) = 0;
	virtual void glVertices(const PathObject* prev_po){}
	virtual PathObject *MakeACopy(void)const = 0;
};

class PathLine : public PathObject{
public:
	int GetType(){return 0;}
	void WriteXML(TiXmlNode *root);
	void ReadFromXMLElement(TiXmlElement* pElem);
	void glVertices(const PathObject* prev_po);
	PathObject *MakeACopy(void)const{return new PathLine(*this);}
};

class PathArc : public PathObject{
public:
	double m_c[3]; // defined relative to previous point ( span start point )
	int m_dir; // 1 - anti-clockwise, -1 - clockwise
	PathArc(){m_c[0] = m_c[1] = m_c[2] = 0.0; m_dir = 1;}
	int GetType(){return 1;}
	void WriteXML(TiXmlNode *root);
	void ReadFromXMLElement(TiXmlElement* pElem);
	void glVertices(const PathObject* prev_po);
	PathObject *MakeACopy(void)const{return new PathArc(*this);}
};

class ColouredPath
{
public:
	LinesColorEnum m_color_type;
	std::list< PathObject* > m_points;
	ColouredPath():m_color_type(LinesColorRapidType){}
	ColouredPath(const ColouredPath& c);
	~ColouredPath(){Clear();}

	const ColouredPath &operator=(const ColouredPath& c);

	void Clear();
	void glCommands();
	void GetBox(CBox &box);
	void WriteXML(TiXmlNode *root);
	void ReadFromXMLElement(TiXmlElement* pElem);
};

class CNCCodeBlock:public HeeksObj
{
public:
	std::list<ColouredText> m_text;
	std::list<ColouredPath> m_line_strips;
	long m_from_pos, m_to_pos; // position of block in text ctrl

	CNCCodeBlock():m_from_pos(-1), m_to_pos(-1){}

	// HeeksObj's virtual functions
	int GetType()const{return NCCodeBlockType;}
	HeeksObj *MakeACopy(void)const;
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	void WriteXML(TiXmlNode *root);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	void AppendTextCtrl(wxTextCtrl *textCtrl);
};

class CNCCode:public HeeksObj
{
public:
	static long pos; // used for setting the CNCCodeBlock objects' m_from_pos and m_to_pos
	static HeeksColor m_text_colors[MaxTextColorTypes];
	static HeeksColor m_lines_colors[MaxLinesColorType];
	static std::string m_text_colors_str[MaxTextColorTypes];
	static std::string m_lines_colors_str[MaxLinesColorType];
	std::list<CNCCodeBlock*> m_blocks;
	int m_gl_list;
	CBox m_box;
	CNCCodeBlock* m_highlighted_block;
	bool m_user_edited; // set, if the user has edited the nc code
	static PathObject* prev_po;

	CNCCode();
	CNCCode(const CNCCode &p):m_gl_list(0), m_highlighted_block(NULL){operator=(p);}
	virtual ~CNCCode();

	const CNCCode &operator=(const CNCCode &p);
	void Clear();

	// HeeksObj's virtual functions
	int GetType()const{return NCCodeType;}
	const wxChar* GetTypeString(void)const{return _T("NC Code");}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	wxString GetIcon(){return _T("../HeeksCNC/icons/nccode");}
	void GetProperties(std::list<Property *> *list);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	HeeksObj *MakeACopy(void)const;
	void CopyFrom(const HeeksObj* object);
	void WriteXML(TiXmlNode *root);
	bool CanAdd(HeeksObj* object);
	bool CanAddTo(HeeksObj* owner);
	bool OneOfAKind(){return true;}
	void SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	static void ReadColorsFromConfig();
	static void WriteColorsToConfig();
	static void GetOptions(std::list<Property *> *list);

	void DestroyGLLists(void); // not void KillGLLists(void), because I don't want the display list recreated on the Redraw button
	void SetTextCtrl(wxTextCtrl *textCtrl);
	void HighlightBlock(long pos);
};