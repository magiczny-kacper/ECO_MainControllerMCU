# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: swdl.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\nswdl.proto\x12\x04swdl\"b\n\x0b\x46rameHeader\x12%\n\x02id\x18\x01 \x01(\x0e\x32\x19.swdl.FrameHeader.FrameId\",\n\x07\x46rameId\x12\x0f\n\nINFO_FRAME\x10\xa0\x01\x12\x10\n\x0b\x43HUNK_FRAME\x10\xb5\x01\"\x9d\x01\n\tInfoFrame\x12)\n\x08\x62lock_id\x18\x01 \x01(\x0e\x32\x17.swdl.InfoFrame.BlockId\x12\x11\n\tfile_size\x18\x02 \x01(\x07\"R\n\x07\x42lockId\x12\x13\n\x0fMCU_APPLICATION\x10\x00\x12\x19\n\x15SMALL_HMI_APPLICATION\x10\x01\x12\x17\n\x13\x42IG_HMI_APPLICATION\x10\x02\"5\n\tDataFrame\x12\x0c\n\x04size\x18\x01 \x01(\r\x12\r\n\x05\x63hunk\x18\x02 \x01(\x0c\x12\x0b\n\x03\x63rc\x18\x03 \x01(\x07\"l\n\tSWDLFrame\x12!\n\x06header\x18\x01 \x01(\x0b\x32\x11.swdl.FrameHeader\x12\x1d\n\x04info\x18\x02 \x01(\x0b\x32\x0f.swdl.InfoFrame\x12\x1d\n\x04\x64\x61ta\x18\x03 \x01(\x0b\x32\x0f.swdl.DataFrame\"o\n\rResponseFrame\x12*\n\x06status\x18\x01 \x01(\x0e\x32\x1a.swdl.ResponseFrame.Status\"2\n\x06Status\x12\x13\n\x0f\x46RAME_CORRUPTED\x10\x00\x12\x07\n\x02OK\x10\xaa\x01\x12\n\n\x06NOT_OK\x10U')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'swdl_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _globals['_FRAMEHEADER']._serialized_start=20
  _globals['_FRAMEHEADER']._serialized_end=118
  _globals['_FRAMEHEADER_FRAMEID']._serialized_start=74
  _globals['_FRAMEHEADER_FRAMEID']._serialized_end=118
  _globals['_INFOFRAME']._serialized_start=121
  _globals['_INFOFRAME']._serialized_end=278
  _globals['_INFOFRAME_BLOCKID']._serialized_start=196
  _globals['_INFOFRAME_BLOCKID']._serialized_end=278
  _globals['_DATAFRAME']._serialized_start=280
  _globals['_DATAFRAME']._serialized_end=333
  _globals['_SWDLFRAME']._serialized_start=335
  _globals['_SWDLFRAME']._serialized_end=443
  _globals['_RESPONSEFRAME']._serialized_start=445
  _globals['_RESPONSEFRAME']._serialized_end=556
  _globals['_RESPONSEFRAME_STATUS']._serialized_start=506
  _globals['_RESPONSEFRAME_STATUS']._serialized_end=556
# @@protoc_insertion_point(module_scope)
