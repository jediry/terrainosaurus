<?xml version="1.0"?> 

<!DOCTYPE xsl:stylesheet [
  <!ENTITY nbsp "&amp;nbsp;">
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" omit-xml-declaration="yes" indent="yes"/>

  <xsl:include href="./References2HTML.xsl"/>

  <xsl:template match="/document">
    <html>
      <head>
        <title> <xsl:value-of select="title"/> </title>
        <style>
          html { width: 8.5in; }
          body {
              background-color: #EEEEAA;
              color: black;
              font-family: "Arial Narrow";
              font-size: 11pt;
              padding: 0in;
              margin: 1.25in 1.4in 1.25in 1.15in;
          }
          p {
              line-height: 1.5;
              padding: 1pt;
              margin: 2pt 2pt 6pt 2pt;
          }
          .title {
              font-size: x-large;
              font-weight: bold;
              text-align: center;
          }
          .subtitle {
              font-size: large;
              text-align: center;
          }
          .author {
              font-style: italic;
              text-align: center;
          }
          .version {
              text-align: center;
          }

          h1, h2, h3, h4 { margin-bottom: 2pt; font-weight: bold; }
          h1 { margin-top: 8pt; font-size: 14pt; }
          h2 { margin-top: 6pt; font-size: 13pt; }
          h3 { margin-top: 4pt; font-size: 12pt; }
          h4 { margin-top: 2pt; font-size: 11pt; }
          .level1, h2 { margin-left: 20px; margin-right: 20px; }
          .level2, h3 { margin-left: 40px; margin-right: 40px; }
          .level3, h4 { margin-left: 60px; margin-right: 60px; }
          .level4     { margin-left: 80px; margin-right: 80px; }

          li {
              list-style-position: outside;
              list-style-type: square;
          }
        </style>
      </head>
      <body>
        <xsl:apply-templates/>
        <h1> References </h1>
        <xsl:apply-templates select="document('References.xml')/references"/>
      </body>
    </html>
  </xsl:template>


  <xsl:template match="/document/title">
    <div class="title"> <xsl:apply-templates/> </div>
  </xsl:template>

  <xsl:template match="/document/subtitle">
    <div class="subtitle"> <xsl:apply-templates/> </div>
  </xsl:template>

  <xsl:template match="/document/author">
    <div class="author"> <xsl:apply-templates/> </div>
  </xsl:template>

  <xsl:template match="/document/version">
    <div class="version"> <xsl:apply-templates/> </div>
  </xsl:template>


  <xsl:template match="/document/section/title">
    <h1> <xsl:apply-templates/> </h1>
  </xsl:template>

  <xsl:template match="/document/section/section/title">
    <h2> <xsl:apply-templates/> </h2>
  </xsl:template>

  <xsl:template match="/document/section/section/section/title">
    <h3> <xsl:apply-templates/> </h3>
  </xsl:template>

  <xsl:template match="/document/section/section/section/section/title">
    <h4> <xsl:apply-templates/> </h4>
  </xsl:template>


  <xsl:template match="/document/section/paragraph">
    <p class="level1"> <xsl:apply-templates/> </p>
  </xsl:template>

  <xsl:template match="/document/section/section/paragraph">
    <p class="level2"> <xsl:apply-templates/> </p>
  </xsl:template>

  <xsl:template match="/document/section/section/section/paragraph">
    <p class="level3"> <xsl:apply-templates/> </p>
  </xsl:template>

  <xsl:template match="/document/section/section/section/section/paragraph">
    <p class="level4"> <xsl:apply-templates/> </p>
  </xsl:template>


  <xsl:template match="/document/section/list">
    <ul class="level1"> <xsl:apply-templates/> </ul>
  </xsl:template>

  <xsl:template match="/document/section/section/list">
    <ul class="level2"> <xsl:apply-templates/> </ul>
  </xsl:template>

  <xsl:template match="/document/section/section/section/list">
    <ul class="level3"> <xsl:apply-templates/> </ul>
  </xsl:template>

  <xsl:template match="/document/section/section/section/section/list">
    <ul class="level4"> <xsl:apply-templates/> </ul>
  </xsl:template>

  <xsl:template match="item">
    <li> <xsl:apply-templates/> </li>
  </xsl:template>


  <xsl:template match="term">
    <b><i> <xsl:apply-templates/> </i></b>
  </xsl:template>


  <xsl:template match="citation">
    [<xsl:call-template name="resolve-reference"> <xsl:with-param name="id" select="normalize-space(@ref)"/> </xsl:call-template>]
  </xsl:template>

  <xsl:template name="resolve-reference">
    <xsl:param name="id"/>
    <xsl:variable name="reference" select="document('References.xml')//reference[@id = $id]"/>
    <xsl:choose>
      <xsl:when test="not($reference)">
        <span style="color: red; font-weight: bold;">Broken link '<xsl:value-of select="$id"/>'</span>
      </xsl:when>
      <xsl:when test="count($reference/author) > 0">
        <xsl:call-template name="format-authors">
          <xsl:with-param name="author-list" select="$reference/author"/>
          <xsl:with-param name="full-name" select="false()"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="normalize-space($reference/title)"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text disable-output-escaping="yes">&nbsp;</xsl:text>
    <xsl:value-of select="normalize-space($reference/date/year)"/>
  </xsl:template>

</xsl:stylesheet>
