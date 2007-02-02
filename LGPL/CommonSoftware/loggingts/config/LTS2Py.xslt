<!-- created by Nicolas Barriga-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
        <xsl:output method="text" version="1.0" encoding="ASCII"/>
        <xsl:template match="/LogDefinitionType">
<xsl:text>#! /usr/bin/env python
#*******************************************************************************
# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2007
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
#
# "@(#) $Id: LTS2Py.xslt,v 1.3 2007/02/02 10:23:31 nbarriga Exp $"
#
# who       when      what
# --------  --------  ----------------------------------------------
# nbarriga  2007-01-29  created
#
######################################################################
'''
Some form of custom documentation goes here...
'''
######################################################################
from Acspy.Common.Log import getLogger
import ACSLog
from socket import gethostname
from Acspy.Common.TimeHelper import TimeUtil
import time
from os import getpid
from traceback import extract_stack
######################################################################
</xsl:text>
        <xsl:for-each select="LogDefinition">
		<xsl:text>class </xsl:text>
        	<xsl:variable name="logName"><xsl:value-of select="@logName"/></xsl:variable>
		<xsl:value-of select="$logName"/><xsl:text>:
	def __init__(self):
		self._members={}
		self._logger=getLogger("loggingts --")
		self.name="</xsl:text><xsl:value-of select="$logName"/><xsl:text>"
		self.shortDescription="</xsl:text><xsl:value-of select="@shortDescription"/><xsl:text>"
                self.priority=ACSLog.ACS_LOG_</xsl:text><xsl:value-of select="@priority"/><xsl:text>

	def log(self):
		msg=self.shortDescription
		data=[ACSLog.NVPair("logName",self.name)]
		for key, value in self._members.items():
			data.append(ACSLog.NVPair(str(key),str(value)))
		cur_stack=extract_stack()
		rtCont=ACSLog.RTContext("",str(getpid()),str(gethostname()).replace("&lt;", "").replace(">", ""),"","")
		srcInfo=ACSLog.SourceInfo(str(cur_stack[0][0]),str(cur_stack[0][2]),long(cur_stack[0][1]))
		timestamp=TimeUtil().py2epoch(time.time()).value
		self._logger.logTypeSafe(self.priority, timestamp, msg, rtCont, srcInfo, data)

</xsl:text>
		<xsl:for-each select="Member">
		<xsl:text>	def set</xsl:text><xsl:variable name="memberName"><xsl:value-of select="@name"/></xsl:variable>
		<xsl:value-of select="$memberName"/>
		<xsl:text>(self, value):
		try:
			self._members["</xsl:text><xsl:value-of select="$memberName"/><xsl:text>"]=str(value)
		except KeyError, e:
			self._members["</xsl:text><xsl:value-of select="$memberName"/><xsl:text>"].update(str(value))

</xsl:text>
		</xsl:for-each>
	</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
