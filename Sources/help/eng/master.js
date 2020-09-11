//This is the master JavaScript file for the HTML Help documentation.

/* These functions (doSection, noSection) are used to make sidebars appear and disappear.
*/

function doSection (secNum){
	//display the section if it's not displayed; hide it if it is displayed
	if (secNum.style.display=="none"){secNum.style.display=""}
	else{secNum.style.display="none"}
}

function noSection (secNum){
	//remove the section when user clicks in the opened DIV
	if (secNum.style.display==""){secNum.style.display="none"}
}

function doExpand(paraNum,arrowNum){
	//in Menu commands pages: expand the paragraph and rotate the arrow; collapse and rotate it back
	if (paraNum.style.display=="none"){paraNum.style.display="";arrowNum.src="../images/menuardn.gif"}
	else{paraNum.style.display="none";arrowNum.src="../images/menuarrt.gif"}
}

function doExpandContents(paraNum,arrowNum){
	//in Contents pages: expand the paragraph and rotate the arrow; collapse and rotate it back
	if (paraNum.style.display=="none"){paraNum.style.display="";arrowNum.src="../images/contentsardn.gif"}
	else{paraNum.style.display="none";arrowNum.src="../images/contentsarrt.gif"}
}

function doExpandInfoPopup(paraNum,arrowNum){
	//in subdirs of ILWIS, ILWISAPP, ILWISMEN: expand the paragraph and rotate the arrow; collapse and rotate it back
	if (paraNum.style.display=="none"){paraNum.style.display="";arrowNum.src="../../images/contentsardn.gif"}
	else{paraNum.style.display="none";arrowNum.src="../../images/contentsarrt.gif"}
}

//These functions control the behavior of the homepage go arrows.
function liteGo(spNo){
	spNo.style.background="#cc0033";
	spNo.style.color="#FFFFFF";
}

function liteOff(spNo){
	spNo.style.background="transparent";
	spNo.style.color="#000000";
}

//Insert new functions here. Please use unique identifiers and comment liberally.

//find relative path of chm file and link to a file (eg PDF) that is in same directory as chm file

function linkoutchm(fn) {
var X, Y, sl, a, ra, link;
ra = /:/;
a = location.href.search(ra);
if (a == 2)
X = 14;
else
X = 7;
sl = "\\";
Y = location.href.lastIndexOf(sl) + 1;
link = 'file:///' + location.href.substring(X, Y) + fn;
location.href = link;
}