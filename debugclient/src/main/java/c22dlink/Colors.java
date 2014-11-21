// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: colors.proto

package c22dlink;

public final class Colors {
  private Colors() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
  }
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_RGB_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_RGB_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_YUV_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_YUV_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_ColorInfo_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_ColorInfo_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_ColorsInfo_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_ColorsInfo_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_RequestColors_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_RequestColors_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_SaveColors_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_SaveColors_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_c22dlink_LoadColors_descriptor;
  static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
      internal_static_c22dlink_LoadColors_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\014colors.proto\022\010c22dlink\"&\n\003RGB\022\t\n\001r\030\001 \002" +
      "(\005\022\t\n\001g\030\002 \002(\005\022\t\n\001b\030\003 \002(\005\"&\n\003YUV\022\t\n\001y\030\001 \002" +
      "(\005\022\t\n\001u\030\002 \002(\005\022\t\n\001v\030\003 \002(\005\"\241\001\n\tColorInfo\022\n" +
      "\n\002id\030\001 \002(\005\022\032\n\003rgb\030\002 \001(\0132\r.c22dlink.RGB\022\014" +
      "\n\004name\030\003 \001(\t\022\r\n\005merge\030\004 \002(\001\022\020\n\010expected\030" +
      "\005 \002(\005\022\035\n\006yuvLow\030\006 \002(\0132\r.c22dlink.YUV\022\036\n\007" +
      "yuvHigh\030\007 \002(\0132\r.c22dlink.YUV\"1\n\nColorsIn" +
      "fo\022#\n\006colors\030\001 \003(\0132\023.c22dlink.ColorInfo\"" +
      "\017\n\rRequestColors\"\036\n\nSaveColors\022\020\n\010filena" +
      "me\030\001 \002(\t\"\036\n\nLoadColors\022\020\n\010filename\030\001 \002(\t",
      "B\002P\001"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
        new com.google.protobuf.Descriptors.FileDescriptor.    InternalDescriptorAssigner() {
          public com.google.protobuf.ExtensionRegistry assignDescriptors(
              com.google.protobuf.Descriptors.FileDescriptor root) {
            descriptor = root;
            return null;
          }
        };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
        }, assigner);
    internal_static_c22dlink_RGB_descriptor =
      getDescriptor().getMessageTypes().get(0);
    internal_static_c22dlink_RGB_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_RGB_descriptor,
        new java.lang.String[] { "R", "G", "B", });
    internal_static_c22dlink_YUV_descriptor =
      getDescriptor().getMessageTypes().get(1);
    internal_static_c22dlink_YUV_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_YUV_descriptor,
        new java.lang.String[] { "Y", "U", "V", });
    internal_static_c22dlink_ColorInfo_descriptor =
      getDescriptor().getMessageTypes().get(2);
    internal_static_c22dlink_ColorInfo_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_ColorInfo_descriptor,
        new java.lang.String[] { "Id", "Rgb", "Name", "Merge", "Expected", "YuvLow", "YuvHigh", });
    internal_static_c22dlink_ColorsInfo_descriptor =
      getDescriptor().getMessageTypes().get(3);
    internal_static_c22dlink_ColorsInfo_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_ColorsInfo_descriptor,
        new java.lang.String[] { "Colors", });
    internal_static_c22dlink_RequestColors_descriptor =
      getDescriptor().getMessageTypes().get(4);
    internal_static_c22dlink_RequestColors_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_RequestColors_descriptor,
        new java.lang.String[] { });
    internal_static_c22dlink_SaveColors_descriptor =
      getDescriptor().getMessageTypes().get(5);
    internal_static_c22dlink_SaveColors_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_SaveColors_descriptor,
        new java.lang.String[] { "Filename", });
    internal_static_c22dlink_LoadColors_descriptor =
      getDescriptor().getMessageTypes().get(6);
    internal_static_c22dlink_LoadColors_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessage.FieldAccessorTable(
        internal_static_c22dlink_LoadColors_descriptor,
        new java.lang.String[] { "Filename", });
  }

  // @@protoc_insertion_point(outer_class_scope)
}
