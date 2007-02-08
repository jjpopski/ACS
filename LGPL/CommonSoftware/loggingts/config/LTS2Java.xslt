<!-- created by Nicolas Barriga-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:loggingts="Alma/ACSLogTS" xmlns:redirect="http://xml.apache.org/xalan/redirect" extension-element-prefixes="redirect">
        <xsl:output method="text" version="1.0" encoding="ASCII"/>
        <xsl:template match="/loggingts:LogDefinitionType">
	<xsl:variable name="logdefname">
		<xsl:value-of select="@name"/>
	</xsl:variable>
        <xsl:for-each select="loggingts:LogDefinition">
	<xsl:variable name="FileName">
		<xsl:text>alma/acs/</xsl:text><xsl:value-of select="$logdefname"/><xsl:text>/</xsl:text><xsl:value-of select="@logName"/><xsl:text>.java</xsl:text>
	</xsl:variable>
	<redirect:write select="$FileName">
	<xsl:text>/*
*ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2007
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

/**
 * @author  nbarriga
 * @version $Id: LTS2Java.xslt,v 1.1 2007/02/08 11:42:01 nbarriga Exp $
 * @since
 */

package alma.acs.</xsl:text><xsl:value-of select="$logdefname"/>
	<xsl:text>;

import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.HashMap;
import java.util.Map;

import alma.acs.logging.AcsLogLevel;
import alma.acs.logging.AcsLogger;
</xsl:text>
		<xsl:text>public class </xsl:text>
        	<xsl:variable name="logName"><xsl:value-of select="@logName"/></xsl:variable>
		<xsl:value-of select="$logName"/><xsl:text>{
	private Logger m_logger;
	private Map&lt;String, Object> nameValue;
	public </xsl:text><xsl:value-of select="$logName"/><xsl:text>(Logger logger){
		this.m_logger=logger;
		((AcsLogger)m_logger).addLoggerClass(this.getClass());
		nameValue = new HashMap&lt;String, Object>();
		nameValue.put("logName","</xsl:text><xsl:value-of select="$logName"/><xsl:text>");
	}
	public void log(){
		m_logger.log(AcsLogLevel.</xsl:text><xsl:value-of select="@priority"/><xsl:text>,"</xsl:text><xsl:value-of select="@shortDescription"/><xsl:text>",nameValue);
	}
</xsl:text>
		<xsl:for-each select="loggingts:Member">
		<xsl:text>	public void set</xsl:text><xsl:value-of select="@name"/>
		<xsl:text>(</xsl:text><xsl:value-of select="@type"/><xsl:text> value){
		nameValue.put("</xsl:text><xsl:value-of select="@name"/><xsl:text>",value);
	}
</xsl:text>	
		</xsl:for-each>
<xsl:text>
}

</xsl:text>
</redirect:write>
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
